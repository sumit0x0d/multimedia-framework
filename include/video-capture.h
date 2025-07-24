#ifndef VIDEO_CAPTURE_H
#define VIDEO_CAPTURE_H

#include <stdint.h>

typedef void * VideoCaptureBackend;
typedef struct video_capture VideoCapture;

typedef enum video_capture_backend_type {
     VIDEO_CAPTURE_BACKEND_TYPE_PIPEWIRE,
     VIDEO_CAPTURE_BACKEND_TYPE_DRM,
     VIDEO_CAPTURE_BACKEND_TYPE_XCB
} VideoCaptureBackendType;

typedef struct video_capture_create_config {
     VideoCaptureBackendType backend_type;
} VideoCaptureCreateConfig;

typedef struct video_capture_interface {
     void *(*create)(const VideoCaptureCreateConfig *vccconfig);
     void (*destroy)(VideoCaptureBackend vcbackend);
} VideoCaptureInterface;

struct video_capture {
     VideoCaptureBackend backend;
     VideoCaptureInterface interface;
};

VideoCapture *video_capture_create(const VideoCaptureCreateConfig *vccconfig);
void video_capture_destroy(VideoCapture *vcapture);

#endif
