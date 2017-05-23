#include <inttypes.h>

typedef struct keyword keyword;

typedef struct photo photolist;

photolist * InitPhotoList(void);

photolist * NewPhoto(photolist * head, uint32_t new_id_photo, char * new_file_name);

void PrintPhotoList(photolist * head);

void FreePhotoList(photolist * head);
