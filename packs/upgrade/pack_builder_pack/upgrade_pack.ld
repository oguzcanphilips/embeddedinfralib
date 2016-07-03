SECTIONS
{
    . = 0x08020000;
    .data : {
        *(.upgrade_pack)
    }
}
