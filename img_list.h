#include <inttypes.h>

typedef struct keyword keyword;

typedef struct photo photolist;

photolist * InitPhotoList(void);

photolist * NewPhoto(photolist * head, uint32_t new_id_photo, char * new_file_name);

photolist *GetPhoto(photolist *head, uint32_t id);

void PrintPhotoList(photolist * head);

void FreePhotoList(photolist * head);

keyword *NewKeyWord(keyword *key_head, char *new_key_name);

void PrintKeyWords(photolist *k_head);

keyword *GetKeyHead(photolist *head);

void Adding(photolist *aux, keyword *k_head);

int SearchPhotosbyKeyWords(photolist *head, char *kword, uint32_t *photos);

void FreeKeywords(keyword * head);

photolist * DeletePhoto(photolist *head, photolist *aux2);