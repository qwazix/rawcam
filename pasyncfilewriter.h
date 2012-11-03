#ifndef PASYNCFILEWRITER_H
#define PASYNCFILEWRITER_H

#include <queue>
#include <semaphore.h>
#include <string>
#include <pthread.h>
#include <QObject>
#include <QString>

#include "FCam/Frame.h"

//! \file
//! AsyncFile contains classes to load and save images in the background

class Lens;
class Flash;

/** The pAsyncFileWriter saves frames in a low priority background
 * thread */
class pAsyncFileWriter : public QObject{
     Q_OBJECT;
public:
    pAsyncFileWriter();
    ~pAsyncFileWriter();

    /** Save a DNG in a background thread. */
    void saveDNG(FCam::Frame, std::string filename);
    void saveDNG(FCam::Image, std::string filename);

    /** Save a JPEG in a background thread. You can optionally
     * pass a jpeg quality (0-100). */
    void saveJPEG(FCam::Frame, std::string filename, int quality = 75);
    void saveJPEG(FCam::Image, std::string filename, int quality = 75);

    /** Save a raw dump in a background thread. */
    void saveDump(FCam::Frame, std::string filename);
    void saveDump(FCam::Image, std::string filename);

    /** How many save requests are pending (including the one
     * currently saving) */
    int savesPending() {return pending;}

    /** Cancel all outstanding requests. The writer will finish
     * saving the current request, but not save any more */
    void cancel();

private:

    friend void *launch_async_file_writer_thread_(void *);

    struct SaveRequest {
        // only one of these two things should be defined
        FCam::Frame frame;
        FCam::Image image;

        std::string filename;
        enum {DNGFrame = 0, JPEGFrame, JPEGImage, DumpFrame, DumpImage} fileType;
        int quality;
    };

    std::queue<SaveRequest> saveQueue;
    pthread_mutex_t saveQueueMutex;
    sem_t *saveQueueSemaphore;

    bool running, stop;
    pthread_t thread;

    void run();

    int pending;

signals:
    void pictureSaved(QString);
    void queueEmpty();

public slots:
    void finish();

};




#endif
