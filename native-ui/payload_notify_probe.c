#include <stdbool.h>
#define SCE_NOTIFICATION_LOCAL_USER_ID_SYSTEM 0xFE
int sceNotificationSend(int userId, bool isLogged, const char* payload);
static const char toast[] =
"{\n"
"  \"rawData\": {\n"
"    \"viewTemplateType\": \"InteractiveToastTemplateB\",\n"
"    \"channelType\": \"Downloads\",\n"
"    \"useCaseId\": \"IDC\",\n"
"    \"toastOverwriteType\": \"No\",\n"
"    \"isImmediate\": true,\n"
"    \"priority\": 100,\n"
"    \"viewData\": {\n"
"      \"message\": {\"body\": \"Goldengames RetroHub probe executed\"},\n"
"      \"subMessage\": {\"body\": \"PS5 payload main() reached successfully\"}\n"
"    },\n"
"    \"platformViews\": {\n"
"      \"previewDisabled\": {\n"
"        \"viewData\": {\n"
"          \"message\": {\"body\": \"Goldengames RetroHub probe executed\"}\n"
"        }\n"
"      }\n"
"    }\n"
"  },\n"
"  \"localNotificationId\": \"786420026\"\n"
"}";
int main(void){return sceNotificationSend(SCE_NOTIFICATION_LOCAL_USER_ID_SYSTEM,true,toast);}
