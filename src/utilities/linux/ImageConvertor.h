#ifndef IMAGECONVERTOR_H
#define IMAGECONVERTOR_H

class ImageConvertor
{
public:
    typedef enum
    {
        YV12_FORMAT = 0
    } SourceFormat;

    static bool convertToRgb(SourceFormat srcType,
                             unsigned char *sourceBuffer, int sourceWidth, int sourceHight,
                             unsigned char **destinationBuffer );

private:
    /**
     * @brief conversion function used in the screen shot feature.
     */

//     void     yuy2toyv12 (uint8_t *y, uint8_t *u, uint8_t *v, uint8_t *input, int width, int height);

    /**
     * @see yuy2toyv12
     */

//     uint8_t *yv12torgb (uint8_t *src_y, uint8_t *src_u, uint8_t *src_v, int width, int height);
};

#endif // IMAGECONVERTOR_H
