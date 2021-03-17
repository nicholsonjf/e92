uint8_t data[512];
struct sdhc_card_status card_status;
enum sdhc_status status;

status = sdhc_read_single_block(rca, sector, &card_status, data);

if (status != SDHC_SUCCESS) {
    return E_MICRO_SD;
}