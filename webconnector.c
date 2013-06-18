#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <nfc/nfc.h>
#include <unistd.h>
#include <string.h>

static int keepRunning = 1;

void sig_handler(int signo)
{
    if (signo == SIGINT)
    {
        printf("Caught SigHandler");
        keepRunning = 0;
    }
}

static void print_hex(const uint8_t *pbtData, const size_t szBytes)
{
    size_t    szPos;

    for (szPos = 0; szPos < szBytes; szPos++) {
        printf("%02x    ", pbtData[szPos]);
    }
    printf("\n");
}

int main(int argc, const char *argv[])
{
    nfc_device *pnd;
    nfc_target nt;
    char currUID[80];
    char lastUID[80];
    char *pCurrUID = currUID;
    char *pLastUID = lastUID;
    int i;

    // Allocate only a pointer to nfc_context
    nfc_context *context;

    // Initialize libnfc and set the nfc_context
    nfc_init(&context);
    if (context == NULL) {
        printf("Unable to init libnfc (malloc)\n");
        exit(EXIT_FAILURE);
    }

    // Display libnfc version
    const char *acLibnfcVersion = nfc_version();
    (void)argc;
    printf("%s uses libnfc %s\n", argv[0], acLibnfcVersion);

    // Open, using the first available NFC device which can be in order of selection:
    // - default device specified using environment variable or
    // - first specified device in libnfc.conf (/etc/nfc) or
    // - first specified device in device-configuration directory (/etc/nfc/devices.d) or
    // - first auto-detected (if feature is not disabled in libnfc.conf) device
    pnd = nfc_open(context, NULL);

    if (pnd == NULL) {
        printf("ERROR: %s", "Unable to open NFC device.");
        exit(EXIT_FAILURE);
    }
    // Set opened NFC device to initiator mode
    if (nfc_initiator_init(pnd) < 0) {
        nfc_perror(pnd, "nfc_initiator_init");
        exit(EXIT_FAILURE);
    }

    printf("NFC reader: %s opened\n", nfc_device_get_name(pnd));

    while(keepRunning)
    {
        if (signal(SIGINT, sig_handler) == SIG_ERR)
            printf("\ncan't catch SIGINT\n");

        // Poll for a ISO14443A (MIFARE) tag
        const nfc_modulation nmMifare = {
            .nmt = NMT_ISO14443A,
            .nbr = NBR_106,
        };

        if (nfc_initiator_select_passive_target(pnd, nmMifare, NULL, 0, &nt) > 0)
        {
            pCurrUID = currUID;
            for(i = 0; i < nt.nti.nai.szUidLen; i++)
            {
                pCurrUID += sprintf(pCurrUID, "%d", nt.nti.nai.abtUid[i]);
            }

            if(strncmp(currUID, lastUID, sizeof(currUID)))
            {
                /* New Card! */
                printf("UID: %s\n", currUID);
                strncpy(lastUID, currUID, sizeof(currUID));
            }
        }
    }

    // Close NFC device
    nfc_close(pnd);
    // Release the context
    nfc_exit(context);
    exit(EXIT_SUCCESS);
}