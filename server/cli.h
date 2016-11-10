
typedef struct COMMAND_TABLE {
	char         *command;
	char         *description;
	char         **syntax;
	int          (*procedure)(int, char **);
}COMMAND_TABLE_t;

enum    STRCMP_RC {
    STRCMP_OK = 0,
    STRCMP_FAIL = 1
};
#define isalpha(c)      (((c >= 'A') && (c <= 'Z')) || ((c >= 'a') && (c <= 'z')))
#define isupper(c)      ( (c >= 'A') && (c <= 'Z') )
#define islower(c)      ( (c >= 'a') && (c <= 'z') )
#define isdigit(c)      ( (c >= '0') && (c <= '9') )
#define toupper(c)      ( islower(c) ? (c - 'a' + 'A'):c)
#define tolower(c)      ( isupper(c) ? (c - 'A' + 'a'):c)

extern int strcmp_nocase(char *str1,char *str2);
