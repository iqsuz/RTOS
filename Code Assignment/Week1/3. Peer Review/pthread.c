#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include <errno.h>



#include <sys/stat.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <fcntl.h>

#include <linux/videodev2.h>


#define CLEAR(x) memset((x), 0, sizeof(*x))

struct buffer{
    void *start;
    size_t length;
};




#include "capture.h"


int open_capture_device(const char *dev_name, int *fd)
{
    struct stat st;

    if(stat(dev_name, &st) == -1)
    {        
        fprintf(stderr, "Cannot identify '%s': %d, %s\n",
                dev_name, errno, strerror(errno));
        exit(EXIT_FAILURE);
    }
    
    if(!S_ISCHR(st.st_mode))
    {
        fprintf(stderr, "%s is no device\n", dev_name);
        exit(EXIT_FAILURE);
    }

    if((*fd = open(dev_name, O_RDWR | O_NONBLOCK, 0)) == -1)
    {
        fprintf(stderr, "Cannot open '%s': %d, %s\n",
                dev_name, errno, strerror(errno));
        exit(EXIT_FAILURE);
    }

    return 0;
}

int init_frame(int fd, struct v4l2_format *fmt)
{
    CLEAR(fmt);


    fmt->type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    fmt->fmt.pix.width = HRES;
    fmt->fmt.pix.height = VRES;
    fmt->fmt.pix.pixelformat = V4L2_PIX_FMT_YUYV;
    fmt->fmt.pix.field = V4L2_FIELD_NONE;

    if(xioctl(fd, VIDIOC_S_FMT, fmt) == -1){
        printf("%s, Frame params couldn't be adjusted.\n",strerror(errno));
        exit(EXIT_FAILURE);
    }

    return 0;
}

int init_mmap(int fd, int buf_size, struct buffer *buffers)
{
    struct v4l2_requestbuffers req;

    CLEAR(req);

    req.count = buf_size;
    req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    req.memory = V4L2_MEMORY_MMAP;

    if(xioctl(fd, VIDIOC_REQBUFS, &req) == -1)
    {
       printf("%s, Memory couldn't be allocated.\n",strerror(errno));
    }
    
    if(req.count < buf_size){
        printf("Buffer size allocated smaller than what requested.");
        exit(EXIT_FAILURE);
    }

    for(int i = 0; i < buf_size; i++){
        struct v4l2_buffer buf;

        CLEAR(buf);

        buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buf.memory = V4L2_MEMORY_MMAP;
        buf.index = i;

        if (-1 == xioctl(fd, VIDIOC_QUERYBUF, &buf)){
            printf("%s - Buffer couldn't retrieved.", strerror(errno));
            exit(EXIT_FAILURE); 
        }

        buffers[i].length = buf.length;
        buffers[i].start = mmap(NULL, buf.length, PROT_READ | PROT_WRITE,
                            MAP_SHARED, fd, buf.m.offset);

        if(buffers[i].start == MAP_FAILED){
            printf("One of allocated memory failed to be mapped.");
            exit(EXIT_FAILURE);
        }

    }

    return 0;
}
