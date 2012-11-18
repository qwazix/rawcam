#include <errno.h>
#include <iostream>

#include "pasyncfilewriter.h"
#include "FCam/Frame.h"
#include "FCam/processing/JPEG.h"
#include "FCam/processing/DNG.h"
#include "FCam/processing/Dump.h"
#include "QString"
#include "QSettings"


using namespace std;

void *launch_async_file_writer_thread_(void *arg) {
    pAsyncFileWriter *d = (pAsyncFileWriter *)arg;
    d->run();
    d->running = false;
    pthread_exit(NULL);
    return NULL;
}


pAsyncFileWriter::pAsyncFileWriter() {
    pthread_attr_t attr;
    struct sched_param param;

    pending = 0;

    // make the thread

    param.sched_priority = sched_get_priority_min(SCHED_OTHER);

    pthread_attr_init(&attr);

    running = false;
    stop = false;

#ifdef FCAM_PLATFORM_OSX
    // unnamed semaphores not supported on OSX
    char semName[256];
    // Create a unique semaphore name for this TSQueue using its pointer value
    snprintf(semName, 256, "FCam::AsyncFile::sem::%llx", (long long unsigned)this);
    saveQueueSemaphore = sem_open(semName, O_CREAT, 666, 0);
#else
    saveQueueSemaphore = new sem_t;
    sem_init(saveQueueSemaphore, 0, 0);
#endif
    pthread_mutex_init(&saveQueueMutex, NULL);

    if ((errno =
             -(pthread_attr_setschedparam(&attr, &param) ||
               pthread_attr_setschedpolicy(&attr, SCHED_OTHER) ||
               pthread_attr_setinheritsched(&attr, PTHREAD_EXPLICIT_SCHED) ||
               pthread_create(&thread, &attr, launch_async_file_writer_thread_, this)))) {
        error(FCam::Event::InternalError, "Error creating async file writer thread");
        return;
    } else {
        running = true;
    }
}

void pAsyncFileWriter::finish(){
    stop = true;
    sem_post(saveQueueSemaphore);
    if (running) {
        pthread_join(thread, NULL);
    }
#ifdef FCAM_PLATFORM_OSX
    sem_close(saveQueueSemaphore);
    char semName[256];
    // Recreate the unique semaphore name for this pAsyncFileWriter using its pointer value
    snprintf(semName, 256, "FCam::AsyncFile::sem::%llx", (long long unsigned)this);
    sem_unlink(semName);
#else
    sem_destroy(saveQueueSemaphore);
    delete saveQueueSemaphore;
#endif
}

pAsyncFileWriter::~pAsyncFileWriter() {
    finish();
}

void pAsyncFileWriter::save(FCam::Frame f){
    QSettings settings;
    char fname[256];
    // Save it as a JPEG
    snprintf(fname, 255, "%s/MyDocs/DCIM/photo_%s.jpg", getenv("HOME"),f.exposureStartTime().toString().c_str());
    saveJPEG(f, fname, 90);
    // Save it as a DNG
    snprintf(fname, 255, "%s/MyDocs/DCIM/photo_%s.dng", getenv("HOME"),f.exposureStartTime().toString().c_str());
    if (settings.value("saveDng",true)== true) {
        saveDNG(f, fname);
    }
}

void pAsyncFileWriter::saveDNG(FCam::Frame f, std::string filename) {
    pending++;
    SaveRequest r;
    r.frame = f;
    r.filename = filename;
    r.fileType = SaveRequest::DNGFrame;
    r.quality = 0; // meaningless for DNG

    pthread_mutex_lock(&saveQueueMutex);
    saveQueue.push(r);
    pthread_mutex_unlock(&saveQueueMutex);
    sem_post(saveQueueSemaphore);
}

void pAsyncFileWriter::saveJPEG(FCam::Frame f, std::string filename, int quality) {
    pending++;
    SaveRequest r;
    r.frame = f;
    r.filename = filename;
    r.quality = quality;
    r.fileType = SaveRequest::JPEGFrame;

    pthread_mutex_lock(&saveQueueMutex);
    saveQueue.push(r);
    pthread_mutex_unlock(&saveQueueMutex);
    sem_post(saveQueueSemaphore);
}

void pAsyncFileWriter::saveJPEG(FCam::Image im, std::string filename, int quality) {
    pending++;
    SaveRequest r;
    r.image = im;
    r.filename = filename;
    r.quality = quality;
    r.fileType = SaveRequest::JPEGImage;

    pthread_mutex_lock(&saveQueueMutex);
    saveQueue.push(r);
    pthread_mutex_unlock(&saveQueueMutex);
    sem_post(saveQueueSemaphore);
}

void pAsyncFileWriter::saveDump(FCam::Frame f, std::string filename) {
    pending++;
    SaveRequest r;
    r.frame = f;
    r.filename = filename;
    r.quality = 0;
    r.fileType = SaveRequest::DumpFrame;

    pthread_mutex_lock(&saveQueueMutex);
    saveQueue.push(r);
    pthread_mutex_unlock(&saveQueueMutex);
    sem_post(saveQueueSemaphore);
}

void pAsyncFileWriter::saveDump(FCam::Image im, std::string filename) {
    pending++;
    SaveRequest r;
    r.image = im;
    r.filename = filename;
    r.quality = 0;
    r.fileType = SaveRequest::DumpImage;

    pthread_mutex_lock(&saveQueueMutex);
    saveQueue.push(r);
    pthread_mutex_unlock(&saveQueueMutex);
    sem_post(saveQueueSemaphore);
}

void pAsyncFileWriter::cancel() {
    pthread_mutex_lock(&saveQueueMutex);
    while (saveQueue.size()) {
        saveQueue.pop();
    };
    pthread_mutex_unlock(&saveQueueMutex);
}

void pAsyncFileWriter::run() {
    while (!stop) {
        sem_wait(saveQueueSemaphore);
        if (stop) { return; }
        SaveRequest r;
        pthread_mutex_lock(&saveQueueMutex);
        r = saveQueue.front();
        saveQueue.pop();
        pthread_mutex_unlock(&saveQueueMutex);
        switch (r.fileType) {
        case SaveRequest::DNGFrame:
            FCam::saveDNG(r.frame, r.filename);
            break;
        case SaveRequest::JPEGFrame:
            FCam::saveJPEG(r.frame, r.filename, r.quality);
            emit pictureSaved(QString::fromStdString(r.filename));
            break;
        case SaveRequest::JPEGImage:
            FCam::saveJPEG(r.image, r.filename, r.quality);
            emit pictureSaved(QString::fromStdString(r.filename));
            break;
        case SaveRequest::DumpFrame:
            FCam::saveDump(r.frame, r.filename);
            break;
        case SaveRequest::DumpImage:
            FCam::saveDump(r.image, r.filename);
            break;
        default:
            cerr << "Corrupted entry in async file writer save queue." << endl;
        }
        pending--;
        if (pending == 0) emit queueEmpty();
    }
}

