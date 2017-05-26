#include <inttypes.h>

typedef struct keyword keyword;

typedef struct photo photolist;

photolist * InitPhotoList(void);

photolist * NewPhoto(photolist * head, uint32_t new_id_photo, char * new_file_name);

photolist *GetPhoto(photolist *head, uint32_t id);

void PrintPhotoList(photolist * head);

void FreePhotoList(photolist * head);

keyword *NewKeyWord(keyword *key_head, char *new_key_name);
