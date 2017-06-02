#include <inttypes.h>

struct photo{    //List that will store photo information
  uint32_t id_photo;
  char *file_name;
  struct keyword * key_head;
  struct photo * next;
};

struct keyword{    //List that will store keyword information
  char *keyword_name;
  struct keyword * next_key;
};

typedef struct keyword keyword;

typedef struct photo photolist;

photolist * InitPhotoList(void);

photolist * NewPhoto(photolist * head, uint32_t new_id_photo, char * new_file_name);

photolist *GetPhoto(photolist *head, uint32_t id);

void PrintPhotoList(photolist * head);

void FreePhotoList(photolist * head);

void NewKeyWord(photolist *aux, char *new_key_name);

void PrintKeyWords(photolist *k_head);

keyword *GetKeyHead(photolist *head);

void Adding(photolist *aux, keyword *k_head);

int SearchPhotosbyKeyWords(photolist *head, char *kword, uint32_t *photos);

void FreeKeywords(keyword * head);

photolist * DeletePhoto(photolist *head, photolist *aux2);

char *GetPhotoName(photolist *head);

photolist *InsertPhotoEnd(photolist *head, uint32_t new_id_photo, char * new_file_name);

uint32_t GetID(photolist *head);

