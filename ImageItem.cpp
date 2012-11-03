#include <QMutex>

#include <FCam/processing/Demosaic.h>
#include <FCam/processing/DNG.h>
#include <FCam/processing/JPEG.h>
#include "ImageItem.h"
#include "QSettings"
#include "QDebug"

#include <iostream>
#include <QDir>

ImageItem::ImageItem() :
    fname(""), saved(false), error(false), lock(QMutex::Recursive) {
    loading = false;
    saving = false;
    loadingThumb = false;
    loadedThumb = false;
    loaded = false;
}

ImageItem::ImageItem(const FCam::Frame &f, bool save = false) :
    src(f), saved(false), error(false), lock(QMutex::Recursive) {
    loading = false;
    saving = false;
    loadingThumb = false;
    loaded = true;
    loadedThumb = false;
    
    char tmpname[256];
    // create filenames
    fpath = QString(getenv("HOME")) + "/MyDocs/DCIM/";
    snprintf(tmpname, 255, "photo_%s.jpg", f.exposureStartTime().toString().c_str());
    fname  = QString(tmpname);

    if (save) this->save();

    if (!src.valid() || !src.image().valid()) error = true;
}


ImageItem::ImageItem(const QString &filename) :
    saved(true), error(false), lock(QMutex::Recursive) {
    int lastSlashIndex = filename.lastIndexOf("/");

    fpath = filename.left(lastSlashIndex+1);
    fname = filename.right(filename.count() - lastSlashIndex);
    fname = fname.left(fname.count() - 4); // 4 because we want to trim ".dng"
    loading = false;
    saving = false;
}

ImageItem::~ImageItem() {

}

FCam::Frame ImageItem::frame() {
    return src;
}

void ImageItem::discardFrame() {
    if (!lock.tryLock()) return;
    if (saved || error) {
        src = FCam::Frame();
        loaded = false;
    }

    demosaicImage = FCam::Image();
    demosaicPixmap.reset();
    
    lock.unlock();
}

void ImageItem::discardThumbnail() {
    if (!lock.tryLock()) return;
    thumb = FCam::Image();
    loadedThumb = false;
    
    pix.reset();
    lock.unlock();
}

QString ImageItem::fullPath() {
    return QString("%1%2.dng").arg(fpath).arg(fname);
}

void ImageItem::load() {
    if (error) return;
    lock.lock();
    if (src.valid() || fname.size() == 0) {
        lock.unlock();
        return;
    }
    FCam::DNGFrame frame = FCam::loadDNG(this->fullPath().toStdString());
    src = frame;
    thumb = frame.thumbnail(); 
    if (src.valid() && !thumb.valid()) thumbnail();
    if (!src.valid()) error = true;
    saved = true;
    loaded = true;
    loadedThumb = true;
    loading = false;
    loadingThumb = false;
    lock.unlock();
}

void ImageItem::loadThumbnail() {
    if (error) return;
    lock.lock();
    if (thumb.valid()) {
        loadingThumb = false;
        loadedThumb = true;
        lock.unlock();
        return;
    }
    FCam::DNGFrame frame = FCam::loadDNG(this->fullPath().toStdString());
    if (frame.valid()) {
        thumb = frame.thumbnail();
        if (!thumb.valid()) {
            // Couldn't find a thumbnail. Try to make one.
            thumb = FCam::makeThumbnail(frame);
        }
    }
    loadingThumb = false;
    loadedThumb = true;
    if (!thumb.valid()) error = true;
    lock.unlock();
}

void ImageItem::loadThumbnailAsync() {
    if (error) return;
    if (thumb.valid()) return;
    if (fname.size() == 0) return;
    loadingThumb = true;
    IOThread::reader().loadThumbnail(this);
}

void ImageItem::loadAsync() {
    if (error) return;
    if (src.valid()) return;
    if (fname.size() == 0) return;
    loading = true;
    loadingThumb = true;
    IOThread::reader().load(this);
}

void ImageItem::save() {
    if (error) return;
    lock.lock();
    if (!src.valid() || saved) {
        lock.unlock();
        return;
    }
    
    QDir dir;
    dir.mkpath(getenv("HOME") + QString("/MyDocs/DCIM/"));
    
    QSettings settings;
    if (settings.value("saveDng",true)== true) writer.saveDNG(src, this->fullPath().toStdString());
    if (1) {
        QString path = "/home/user/MyDocs/DCIM/";
        path.append(fname).append(".jpg");
        writer.saveJPEG(src, path.toStdString());
    }
    saving = false;
    saved = true;
    
    lock.unlock();
}

QString ImageItem::tempJPEGPath(){
    QString path = QDir::tempPath();
    path.append("/").append(fname).append(".jpg");
    if (!src.valid() || !src.image().valid())
        this->load();        
    FCam::saveJPEG(src, path.toStdString());
    return path;
}

bool ImageItem::placeholder() {
    return (fname.count() + fpath.count() == 0);
}

QString ImageItem::saveAsync() {
    qDebug()<<"save async";
    if (!src.valid() || saved) return "invalid";
    saving = true;
    IOThread::writer().save(this);
    return fname;
}

void ImageItem::demosaic() {
    if (!src.valid() || !src.image().valid())
        return;
    FCam::Image demosaicFImage = FCam::demosaic(src);
    QImage demosaicQImage(demosaicFImage(0,0), 
                          demosaicFImage.width(), demosaicFImage.height(), QImage::Format_RGB888);

    QPixmap * tempPixmap = new QPixmap(QPixmap::fromImage(demosaicQImage));
    
    lock.lock();
    demosaicPixmap.reset(tempPixmap);
    lock.unlock();
}

void ImageItem::demosaicAsync() {
    if (!src.valid()) return;
    IOThread::reader().demosaic(this);
}


FCam::Image ImageItem::thumbnail() {
    if (!lock.tryLock()) {
        return FCam::Image();
    }
    if (!thumb.valid() && src.valid()) {
        thumb = FCam::makeThumbnail(src, FCam::Size(640, 480));
    }
    lock.unlock();
    return thumb;
}

const QPixmap ImageItem::pixmap() {
    if (!lock.tryLock()) return QPixmap();
    thumbnail();
    if (!thumb.valid()) {
        lock.unlock();
        return QPixmap();
    } else if (!pix) {
        QImage thumbQ(thumb(0,0), 640, 480, QImage::Format_RGB888);
        pix.reset(new QPixmap(QPixmap::fromImage(thumbQ)));
    } 
    lock.unlock();
    return *pix;
}

const QPixmap ImageItem::fullResPixmap() {    
    lock.lock();
    
    if (!demosaicPixmap || demosaicPixmap->isNull()) {
        if (!src.valid() && !error) {
            this->load();
        } 
        if (src.valid() && src.image().valid()) {
            demosaicPixmap.reset(
                new QPixmap(this->pixmap().scaled(src.image().width(), src.image().height())));
            this->demosaicAsync();
        }
    }
    lock.unlock();
    // TODO: the dng file must be corrupt. Display a precanned invalid image pixmap?
    if (!demosaicPixmap) return this->pixmap();
    return *demosaicPixmap;
}

const QString &ImageItem::filename() {
    return fname;
}

bool ImageItem::safeToDelete() {
    //printf("saving %d (ed) %d loading %d, loadingThumb %d\n", saving, saved, loading, loadingThumb);
    return saved && !saving && !loading && !loadingThumb;
}

void IOThread::stop() {
    queue.push(Request(NULL, Stop));
}


IOThread::IOThread() : QThread(NULL) {
    
}

IOThread &IOThread::writer() {
    static IOThread *_instance = NULL;
    if (!_instance) {
        _instance = new IOThread();
        _instance->start(QThread::IdlePriority);
    }
    return *_instance;
}

IOThread &IOThread::reader() {
    static IOThread *_instance = NULL;
    if (!_instance) {
        _instance = new IOThread();
        _instance->start(QThread::IdlePriority);
    }
    return *_instance;
}

void IOThread::run() {
    while (1) {
        Request r = queue.pull();

        switch (r.type) {
        case Stop:
            return;
        case Load:
            if (queue.size() < 8) {
                // start ignoring old loads if I get hammered with requests
                r.image->load();
                emit loadFinished(r.image);
                break;
            }
        case Save:
            r.image->save();
            emit saveFinished(r.image);
            break;
        case LoadThumbnail:
            if (queue.size() < 8) {
                // start ignoring old loads if I get hammered with requests
                r.image->loadThumbnail();
                emit loadFinished(r.image);
                break;
            }
        case Demosaic:
            r.image->demosaic();
            emit demosaicFinished();        
            break;
        }

    }
}

