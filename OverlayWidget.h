#ifndef OVERLAY_WIDGET_H
#define OVERLAY_WIDGET_H

#include <QWidget>
#include <QX11Info>

#define __user
#include <linux/omapfb.h>
#include <stdio.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include <FCam/Image.h>

class OverlayWidget : public QWidget {
    Q_OBJECT;

public:
    OverlayWidget(QWidget *parent = NULL);
    ~OverlayWidget();

    // If you draw on a widget at the same place as this one, using
    // any color but the one below, it will show through the overlay.
    static QColor colorKey() {return QColor(12, 14, 3);}

    // A reference to the frame buffer
    FCam::Image framebuffer();

public slots:
    void newViewfinderFrame();
    void enable();
    void disable();

signals:
    void activate();
    void deactivate();
    void quit();

protected:
    struct omapfb_color_key old_color_key;
    void resizeEvent(QResizeEvent *);
    void moveEvent(QMoveEvent *);
    void showEvent(QShowEvent *);
    void hideEvent(QHideEvent *);
    bool eventFilter(QObject *receiver, QEvent *event);
    virtual void drawUI() = 0;
    virtual bool inputHandler(QMouseEvent *) = 0;

    FCam::Image framebuffer_;

    unsigned char *ptr;
    unsigned char *cur;
    //struct fb_var_screeninfo var_info;
    struct fb_var_screeninfo overlay_info;
    struct omapfb_mem_info mem_info;
    struct omapfb_plane_info plane_info;
    int overlay_fd;

};

#endif
