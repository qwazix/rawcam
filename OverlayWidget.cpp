#include "OverlayWidget.h"
#include <QEvent>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <QPaintEvent>

#define WIDTH 854
#define HEIGHT 480

OverlayWidget::OverlayWidget(QWidget *par) : QWidget(par)  {
    /* Make QT do the work of keeping the overlay the magic color  */
    QWidget::setBackgroundRole(QPalette::Window);
    QWidget::setAutoFillBackground(true);
    QPalette overlayPalette = QWidget::palette();
    overlayPalette.setColor
    (QPalette::Window,
     colorKey());
    QWidget::setPalette(overlayPalette);

    // Open the overlay device
    overlay_fd = open("/dev/fb1", O_RDWR);

    if (overlay_fd == -1) {
        perror("open");
    }

    // Get the current overlay and plane settings
    if (ioctl(overlay_fd, FBIOGET_VSCREENINFO, &overlay_info)) {
        perror("FBIO_VSCREENINFO");
    }
    if (ioctl(overlay_fd, OMAPFB_QUERY_PLANE, &plane_info)) {
        perror("OMAPFB_QUERY_PLANE");
    }

    // Disable the plane so we can allocate memory for it.
    plane_info.enabled = 0;
    plane_info.pos_x = 0;
    plane_info.pos_y = 0;
    plane_info.out_width = WIDTH;
    plane_info.out_height = HEIGHT;
    if (ioctl(overlay_fd, OMAPFB_SETUP_PLANE, &plane_info)) {
        perror("OMAPFB_SETUP_PLANE");
    }

    // Allocate the memory
    mem_info.size = WIDTH*HEIGHT*2;
    mem_info.type = 0;
    if (ioctl(overlay_fd, OMAPFB_SETUP_MEM, &mem_info)) {
        perror("OMAPFB_SETUP_MEM");
    }

    // mmap it into an FCam image
    ptr = (unsigned char*)mmap(NULL, mem_info.size, PROT_WRITE, MAP_SHARED, overlay_fd, 0);
    if (ptr == MAP_FAILED) {
        perror("mmap");
    }
    cur = (unsigned char*)malloc(WIDTH*HEIGHT*2);
    framebuffer_ = FCam::Image(864, 600, FCam::UYVY);

    // Clear the memory in case there was something hanging around from an earlier invocation
    memset(ptr, 128, WIDTH*HEIGHT*2);

    // Set the overlay properties
    overlay_info.xres = WIDTH;
    overlay_info.yres = HEIGHT;
    overlay_info.xres_virtual = WIDTH;
    overlay_info.yres_virtual = HEIGHT;
    overlay_info.xoffset = 0;
    overlay_info.yoffset = 0;
    overlay_info.nonstd = OMAPFB_COLOR_YUV422;
    if (ioctl(overlay_fd, FBIOPUT_VSCREENINFO, &overlay_info)) {
        perror("FBIOPUT_VSCREENINFO");
    }

    // Set up the color key
    struct omapfb_color_key color_key;
    color_key.key_type = OMAPFB_COLOR_KEY_GFX_DST;
    QColor key = colorKey();
    color_key.trans_key = ((key.red() >> 3) << 11) | ((key.green() >> 2) << 5) | ((key.blue() >> 3));
    if (ioctl(overlay_fd, OMAPFB_SET_COLOR_KEY, &color_key)) {
        perror("OMAPFB_SET_COLOR_KEY");
    }

    // Set the update mode to manual
    int update_mode = OMAPFB_MANUAL_UPDATE;
    if (ioctl(overlay_fd, OMAPFB_SET_UPDATE_MODE, &update_mode) < 0) {
        perror("OMAPFB_SET_UPDATE_MODE");
    }
    installEventFilter(this);
}

bool OverlayWidget::eventFilter(QObject *, QEvent *event) {
    if (event->type() == QEvent::Move ||
        event->type() == QEvent::Resize ||
        event->type() == QEvent::Show ||
	event->type() == QEvent::WindowActivate)
    {
        enable();
	emit activate();
    }
    else if (event->type() == QEvent::Hide ||
	     event->type() == QEvent::WindowDeactivate)
    {
        disable();
	emit deactivate();
    }
    else if (event->type() == QEvent::Close)
    {
	disable();
	emit quit();
    }
    else if (event->type() == QEvent::MouseButtonPress ||
	     event->type() == QEvent::MouseButtonRelease ||
	     event->type() == QEvent::MouseMove)
    {
	    return inputHandler( (QMouseEvent *)event);
    }

    // We don't capture this event, it should be propagated as normal
    return false;
}

void OverlayWidget::showEvent(QShowEvent *) {
    enable();
}

void OverlayWidget::hideEvent(QHideEvent *) {
    disable();
}

void OverlayWidget::resizeEvent(QResizeEvent *) {
    enable();
}

void OverlayWidget::moveEvent(QMoveEvent *) {
    enable();
}


OverlayWidget::~OverlayWidget() {
    disable();
    ::close(overlay_fd);
}

void OverlayWidget::enable() {
    // Shift the plane according to where the widget is, but keep it
    // at 640x480

    QPoint global = mapToGlobal(QPoint(0, 0));

    // round to even X
    global.setX(global.x()/2);
    global.setX(global.x()*2);

    int xoff = global.x() > 0 ? global.x() : 0;
    int yoff = global.y() > 0 ? global.y() : 0;
    int xcrop = global.x() < 0 ? -global.x() : 0;
    int ycrop = global.y() < 0 ? -global.y() : 0;

    if (xcrop > WIDTH || ycrop > HEIGHT) {
        disable();
        return;
    }

    // Set the size and position on screen
    plane_info.enabled = 1;
    plane_info.pos_x = xoff;
    plane_info.pos_y = yoff;
    plane_info.out_width = WIDTH - xcrop;
    plane_info.out_height = HEIGHT - ycrop;

    if (ioctl(overlay_fd, OMAPFB_SETUP_PLANE, &plane_info)) {
        perror("OMAPFB_SETUP_PLANE");
    }

    // The image is always 854x480
    overlay_info.xres_virtual = WIDTH;
    overlay_info.yres_virtual = HEIGHT;
    // Set the portion of it that's visible on screen
    overlay_info.xres = plane_info.out_width;
    overlay_info.yres = plane_info.out_height;
    overlay_info.xoffset = xcrop;
    overlay_info.yoffset = ycrop;
    overlay_info.nonstd = OMAPFB_COLOR_YUV422;
    if (ioctl(overlay_fd, FBIOPUT_VSCREENINFO, &overlay_info)) {
        perror("FBIOPUT_VSCREENINFO");
    }
}

void OverlayWidget::disable() {
    plane_info.enabled = 0;
    if (ioctl(overlay_fd, OMAPFB_SETUP_PLANE, &plane_info)) {
        perror("OMAPFB_SETUP_PLANE");
    }
}

void OverlayWidget::newViewfinderFrame() {
    for (int y = 0; y < HEIGHT; y++) {               
        memcpy(cur + y*WIDTH*2, framebuffer_(0,y), WIDTH*2);
    }
    drawUI();
    memcpy(ptr, cur, WIDTH*HEIGHT*2);
    QPoint global = mapToGlobal(QPoint(0, 0));
    struct omapfb_update_window uw;
    uw.x = global.x();
    uw.y = global.y();
    uw.width = WIDTH;
    uw.height = HEIGHT;
    ioctl(overlay_fd, OMAPFB_UPDATE_WINDOW, &uw);
    ioctl(overlay_fd, OMAPFB_SYNC_GFX);
}


FCam::Image OverlayWidget::framebuffer() {
    return framebuffer_;
}
