#ifndef FCAMERA_IMAGE_ITEM
#define FCAMERA_IMAGE_ITEM

#include <QPixmap>
#include <QMutex>
#include <FCam/Frame.h>
#include <QThread>

#ifdef Q_WS_MAEMO_5
#include <FCam/N900.h>
#define deviceN900 true
#else
#define deviceN900 false
#include <FCam/N9.h>
#endif
#include <iostream>
#include <QDir>

/** A class that represents a displayable image object, which wraps
 * around an FCam::Image and provides demosaiced/downsampled/etc
 * sections of the image on demand, caching results as needed. The
 * CameraThread emits these every time a new photograph is taken.
 */

class ImageItem {
public:
    ImageItem();
    ImageItem(const FCam::Frame &);
    ImageItem(const FCam::Frame &, bool);
    ImageItem(const QString &filename);
    FCam::AsyncFileWriter writer;
    ~ImageItem();    

    // Get at a pixmap of the thumbnail, computing it if necessary
    const QPixmap pixmap();
    
    // Get at a pixmap of the full resolution image, computing if necessary.
    const QPixmap fullResPixmap();
    
    // Get at an FCam::Image of the thumbnail, computing it if necessary
    FCam::Image thumbnail();

    // Get at the raw frame
    FCam::Frame frame(); 

    // Get the filename this image is stored in (or being saved to)
    const QString &filename();

    // If the frame is saved to disk, throw it out in order to reclaim
    // memory. Get it back by calling load or loadasync.
    void discardFrame();

    // Throw out the thumbnail in order to reclaim some memory. Get it
    // back with loadThumbnail or load. 
    void discardThumbnail();

    // Synchronously use the filename to load the frame
    void load();

    // Asynchronously use the filename to load the frame
    void loadAsync(); 

    // Synchronously use the filename to load the thumbnail
    void loadThumbnail();

    // Asynchronously use the filename to load the thumbnail
    void loadThumbnailAsync(); 

    // Synchronously save the frame to the filename
    void save();

    // Asynchronously save the frame to the filename
    QString saveAsync();

    // Is this ImageItem usable?
    bool valid() {return !error;}

    // Is this ImageItem a placeholder (for telling the user there are no images saved)?
    bool placeholder();
    
    // Asynchronously demosaic the RAW image data to update the demosaic pixmap
    void demosaicAsync();
    
    // Synchronously demosaic the RAW image data to update the demosaic pixmap
    void demosaic();
    
    // Save a jpeg image to a temporary directory for uploading to flickr, etc.
    QString tempJPEGPath();
    
    // The full filepath (e.g. /home/user/MyDocs/photo003.dng
    QString fullPath();

    // Returns false if this image item is currently performing some disk I/O (saving or loading). 
    bool safeToDelete();
private:

    // The filename (e.g. photo003)
    QString fname;
    // The filepath (e.g. /home/user/MyDocs/)
    QString fpath;

    // The following are all reference-counted objects, so we're ok to
    // pass ImageItems around by value.

    // Source frame for the item
    FCam::Frame src;
       
    // Is this image stored to disk?
    bool saving;
    bool saved;
    // Is this image loading from disk?
    bool loading;
    bool loaded; // Somewhat redundant with querying src
    bool loadingThumb;
    bool loadedThumb; // Somewhat redundant with querying thumb
    
    // Is this image corrupted and unusable in some way?
    bool error;
    
    // Cached demosaiced/downsampled data
    std::tr1::shared_ptr<QPixmap> demosaicPixmap;
    FCam::Image demosaicImage;
    FCam::Image thumb;
    std::tr1::shared_ptr<QPixmap> pix;    

    // A lock - both the IOThread and also the thumbnail view are
    // fiddling with ImageItems at the same time
    QMutex lock;

};


// A background thread that takes care of all the disk IO
class IOThread : public QThread {
    Q_OBJECT;
public:
    // A pair of instances for writing and reading
    static IOThread &writer();
    static IOThread &reader();

    // Enqueues a request to load an image item's data from disk
    void load(ImageItem *im) {
        queue.push(Request(im, Load));
    }
    
    // Enqueues a request to load an image item's thumbnail data from disk
    void loadThumbnail(ImageItem *im) {
        queue.push(Request(im, LoadThumbnail));
    }
    
    // Enqueues a request to save an image item and it's thumbnail to disk
    void save(ImageItem *im) {
        queue.push(Request(im, Save));
    }
    
    // Enqueues a request to demosaic an image items frame into a displayable pixmap.
    void demosaic(ImageItem *im) {
        queue.push(Request(im, Demosaic));
    }
public slots:
    // Cleans up the IOThread and allows it to return after draining it's pipeline.
    void stop();
    
signals:
    // These signals are emitted when their corresponding requests have been fulfilled.
    void loadFinished(ImageItem *);
    void saveFinished(ImageItem *);
    void demosaicFinished();
    
protected:
    // The main loop for this thread services its queue of I/O requests.
    void run();

    enum RequestType {Load = 0, Save, LoadThumbnail, Stop, Demosaic};   
    
    struct Request {
        Request(ImageItem *im, RequestType t) : image(im), type(t) {}
        ImageItem *image;
        RequestType type;
    };
    
    FCam::TSQueue<Request> queue; 
    
    IOThread();
};



#endif
