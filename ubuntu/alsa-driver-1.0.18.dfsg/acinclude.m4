dnl ALSA soundcard configuration
dnl Find out which cards to compile driver for
dnl Copyright (c) by Jaroslav Kysela <perex@perex.cz>,
dnl                  Anders Semb Hermansen <ahermans@vf.telia.no>

AC_DEFUN([ALSA_TOPLEVEL_INIT], [
	CONFIG_SOUND=""
	CONFIG_HAS_IOMEM=""
	CONFIG_SOUND_OSS_CORE=""
	CONFIG_M68K=""
	CONFIG_SND=""
	CONFIG_SND_TIMER=""
	CONFIG_SND_PCM=""
	CONFIG_SND_HWDEP=""
	CONFIG_SND_RAWMIDI=""
	CONFIG_SND_JACK=""
	CONFIG_SND_SEQUENCER=""
	CONFIG_SND_SEQ_DUMMY=""
	CONFIG_SND_OSSEMUL=""
	CONFIG_SND_MIXER_OSS=""
	CONFIG_SND_PCM_OSS=""
	CONFIG_SND_PCM_OSS_PLUGINS=""
	CONFIG_SND_SEQUENCER_OSS=""
	CONFIG_SND_RTCTIMER=""
	CONFIG_RTC=""
	CONFIG_SND_SEQ_RTCTIMER_DEFAULT=""
	CONFIG_SND_DYNAMIC_MINORS=""
	CONFIG_SND_SUPPORT_OLD_API=""
	CONFIG_SND_VERBOSE_PROCFS=""
	CONFIG_PROC_FS=""
	CONFIG_SND_VERBOSE_PRINTK=""
	CONFIG_SND_DEBUG=""
	CONFIG_SND_DEBUG_VERBOSE=""
	CONFIG_SND_PCM_XRUN_DEBUG=""
	CONFIG_SND_VMASTER=""
	CONFIG_SND_BIT32_EMUL=""
	CONFIG_SND_DEBUG_MEMORY=""
	CONFIG_SND_HPET=""
	CONFIG_HPET=""
	CONFIG_BROKEN=""
	CONFIG_SND_MPU401_UART=""
	CONFIG_SND_OPL3_LIB=""
	CONFIG_SND_OPL4_LIB=""
	CONFIG_SND_VX_LIB=""
	CONFIG_SND_AC97_CODEC=""
	CONFIG_SND_DRIVERS=""
	CONFIG_SND_PCSP=""
	CONFIG_PCSPKR_PLATFORM=""
	CONFIG_X86_PC=""
	CONFIG_HIGH_RES_TIMERS=""
	CONFIG_INPUT=""
	CONFIG_EXPERIMENTAL=""
	CONFIG_SND_DUMMY=""
	CONFIG_SND_VIRMIDI=""
	CONFIG_SND_MTPAV=""
	CONFIG_SND_MTS64=""
	CONFIG_PARPORT=""
	CONFIG_SND_SERIAL_U16550=""
	CONFIG_SND_MPU401=""
	CONFIG_SND_PORTMAN2X4=""
	CONFIG_SND_ML403_AC97CR=""
	CONFIG_XILINX_VIRTEX=""
	CONFIG_SND_AC97_POWER_SAVE=""
	CONFIG_SND_AC97_POWER_SAVE_DEFAULT=""
	CONFIG_SND_SERIALMIDI=""
	CONFIG_SND_LOOPBACK=""
	CONFIG_SND_WSS_LIB=""
	CONFIG_SND_SB_COMMON=""
	CONFIG_SND_SB8_DSP=""
	CONFIG_SND_SB16_DSP=""
	CONFIG_SND_ISA=""
	CONFIG_ISA=""
	CONFIG_ISA_DMA_API=""
	CONFIG_SND_ADLIB=""
	CONFIG_SND_AD1816A=""
	CONFIG_PNP=""
	CONFIG_ISAPNP=""
	CONFIG_SND_AD1848=""
	CONFIG_SND_ALS100=""
	CONFIG_SND_AZT2320=""
	CONFIG_SND_CMI8330=""
	CONFIG_SND_CS4231=""
	CONFIG_SND_CS4232=""
	CONFIG_SND_CS4236=""
	CONFIG_SND_DT019X=""
	CONFIG_SND_ES968=""
	CONFIG_SND_ES1688=""
	CONFIG_SND_ES18XX=""
	CONFIG_SND_SC6000=""
	CONFIG_HAS_IOPORT=""
	CONFIG_SND_GUSCLASSIC=""
	CONFIG_SND_GUSEXTREME=""
	CONFIG_SND_GUSMAX=""
	CONFIG_SND_INTERWAVE=""
	CONFIG_SND_INTERWAVE_STB=""
	CONFIG_SND_OPL3SA2=""
	CONFIG_SND_OPTI92X_AD1848=""
	CONFIG_SND_OPTI92X_CS4231=""
	CONFIG_SND_OPTI93X=""
	CONFIG_SND_MIRO=""
	CONFIG_SND_SB8=""
	CONFIG_SND_SB16=""
	CONFIG_SND_SBAWE=""
	CONFIG_SND_SB16_CSP=""
	CONFIG_PPC=""
	CONFIG_FW_LOADER=""
	CONFIG_SND_SGALAXY=""
	CONFIG_SND_SSCAPE=""
	CONFIG_SND_WAVEFRONT=""
	CONFIG_SND_WAVEFRONT_FIRMWARE_IN_KERNEL=""
	CONFIG_SND_PC98_CS4232=""
	CONFIG_X86_PC9800=""
	CONFIG_SND_MSND_PINNACLE=""
	CONFIG_X86=""
	CONFIG_SND_PCI=""
	CONFIG_PCI=""
	CONFIG_SND_AD1889=""
	CONFIG_SND_ALS300=""
	CONFIG_SND_ALS4000=""
	CONFIG_SND_ALI5451=""
	CONFIG_SND_ATIIXP=""
	CONFIG_SND_ATIIXP_MODEM=""
	CONFIG_SND_AU8810=""
	CONFIG_SND_AU8820=""
	CONFIG_SND_AU8830=""
	CONFIG_SND_AW2=""
	CONFIG_SND_AZT3328=""
	CONFIG_SND_BT87X=""
	CONFIG_SND_BT87X_OVERCLOCK=""
	CONFIG_SND_CA0106=""
	CONFIG_SND_CMIPCI=""
	CONFIG_SND_OXYGEN_LIB=""
	CONFIG_SND_OXYGEN=""
	CONFIG_SND_CS4281=""
	CONFIG_SND_CS46XX=""
	CONFIG_SND_CS46XX_NEW_DSP=""
	CONFIG_SND_CS5530=""
	CONFIG_SND_CS5535AUDIO=""
	CONFIG_X86_64=""
	CONFIG_SND_DARLA20=""
	CONFIG_SND_GINA20=""
	CONFIG_SND_LAYLA20=""
	CONFIG_SND_DARLA24=""
	CONFIG_SND_GINA24=""
	CONFIG_SND_LAYLA24=""
	CONFIG_SND_MONA=""
	CONFIG_SND_MIA=""
	CONFIG_SND_ECHO3G=""
	CONFIG_SND_INDIGO=""
	CONFIG_SND_INDIGOIO=""
	CONFIG_SND_INDIGODJ=""
	CONFIG_SND_EMU10K1=""
	CONFIG_SND_EMU10K1X=""
	CONFIG_SND_ENS1370=""
	CONFIG_SND_ENS1371=""
	CONFIG_SND_ES1938=""
	CONFIG_SND_ES1968=""
	CONFIG_SND_FM801=""
	CONFIG_SND_FM801_TEA575X_BOOL=""
	CONFIG_VIDEO_V4L1=""
	CONFIG_SND_FM801_TEA575X=""
	CONFIG_SND_HDA_INTEL=""
	CONFIG_SND_HDA_HWDEP=""
	CONFIG_SND_HDA_INPUT_BEEP=""
	CONFIG_SND_HDA_CODEC_REALTEK=""
	CONFIG_SND_HDA_CODEC_ANALOG=""
	CONFIG_SND_HDA_CODEC_SIGMATEL=""
	CONFIG_SND_HDA_CODEC_VIA=""
	CONFIG_SND_HDA_CODEC_ATIHDMI=""
	CONFIG_SND_HDA_CODEC_NVHDMI=""
	CONFIG_SND_HDA_CODEC_CONEXANT=""
	CONFIG_SND_HDA_CODEC_CMEDIA=""
	CONFIG_SND_HDA_CODEC_SI3054=""
	CONFIG_SND_HDA_GENERIC=""
	CONFIG_SND_HDA_POWER_SAVE=""
	CONFIG_SND_HDA_POWER_SAVE_DEFAULT=""
	CONFIG_SND_HDSP=""
	CONFIG_SND_HDSPM=""
	CONFIG_SND_HIFIER=""
	CONFIG_SND_ICE1712=""
	CONFIG_SND_ICE1724=""
	CONFIG_SND_INTEL8X0=""
	CONFIG_SND_INTEL8X0M=""
	CONFIG_SND_KORG1212=""
	CONFIG_SND_MAESTRO3=""
	CONFIG_SND_MIXART=""
	CONFIG_SND_NM256=""
	CONFIG_SND_PCXHR=""
	CONFIG_SND_RIPTIDE=""
	CONFIG_SND_RME32=""
	CONFIG_SND_RME96=""
	CONFIG_SND_RME9652=""
	CONFIG_SND_SIS7019=""
	CONFIG_SND_SONICVIBES=""
	CONFIG_SND_TRIDENT=""
	CONFIG_SND_VIA82XX=""
	CONFIG_SND_VIA82XX_MODEM=""
	CONFIG_SND_VIRTUOSO=""
	CONFIG_SND_VX222=""
	CONFIG_SND_YMFPCI=""
	CONFIG_SND_PDPLUS=""
	CONFIG_SND_ASIHPI=""
	CONFIG_SND_PPC=""
	CONFIG_PPC64=""
	CONFIG_PPC32=""
	CONFIG_SND_POWERMAC=""
	CONFIG_I2C=""
	CONFIG_PPC_PMAC=""
	CONFIG_SND_POWERMAC_AUTO_DRC=""
	CONFIG_SND_PS3=""
	CONFIG_PS3_PS3AV=""
	CONFIG_SND_PS3_DEFAULT_START_DELAY=""
	CONFIG_SND_AOA=""
	CONFIG_SND_AOA_FABRIC_LAYOUT=""
	CONFIG_SND_AOA_ONYX=""
	CONFIG_I2C_POWERMAC=""
	CONFIG_SND_AOA_TAS=""
	CONFIG_SND_AOA_TOONIE=""
	CONFIG_SND_AOA_SOUNDBUS=""
	CONFIG_SND_AOA_SOUNDBUS_I2S=""
	CONFIG_SND_ARM=""
	CONFIG_ARM=""
	CONFIG_SND_SA11XX_UDA1341=""
	CONFIG_ARCH_SA1100=""
	CONFIG_L3=""
	CONFIG_SND_ARMAACI=""
	CONFIG_ARM_AMBA=""
	CONFIG_SND_PXA2XX_PCM=""
	CONFIG_SND_PXA2XX_LIB=""
	CONFIG_SND_PXA2XX_LIB_AC97=""
	CONFIG_SND_PXA2XX_AC97=""
	CONFIG_ARCH_PXA=""
	CONFIG_SND_S3C2410=""
	CONFIG_ARCH_S3C2410=""
	CONFIG_I2C_SENSOR=""
	CONFIG_SND_PXA2XX_I2SOUND=""
	CONFIG_SND_SPI=""
	CONFIG_SPI=""
	CONFIG_SND_AT73C213=""
	CONFIG_ATMEL_SSC=""
	CONFIG_SND_AT73C213_TARGET_BITRATE=""
	CONFIG_SND_MIPS=""
	CONFIG_MIPS=""
	CONFIG_SND_SGI_O2=""
	CONFIG_SGI_IP32=""
	CONFIG_SND_SGI_HAL2=""
	CONFIG_SGI_HAS_HAL2=""
	CONFIG_SND_AU1X00=""
	CONFIG_SOC_AU1000=""
	CONFIG_SOC_AU1100=""
	CONFIG_SOC_AU1500=""
	CONFIG_SND_SUPERH=""
	CONFIG_SUPERH=""
	CONFIG_SND_AICA=""
	CONFIG_SH_DREAMCAST=""
	CONFIG_SND_USB=""
	CONFIG_USB=""
	CONFIG_SND_USB_AUDIO=""
	CONFIG_SND_USB_USX2Y=""
	CONFIG_ALPHA=""
	CONFIG_SND_USB_CAIAQ=""
	CONFIG_SND_USB_CAIAQ_INPUT=""
	CONFIG_SND_USB_US122L=""
	CONFIG_SND_PCMCIA=""
	CONFIG_PCMCIA=""
	CONFIG_SND_VXPOCKET=""
	CONFIG_SND_PDAUDIOCF=""
	CONFIG_SND_SPARC=""
	CONFIG_SPARC=""
	CONFIG_SND_SUN_AMD7930=""
	CONFIG_SBUS=""
	CONFIG_SND_SUN_CS4231=""
	CONFIG_SND_SUN_DBRI=""
	CONFIG_SND_GSC=""
	CONFIG_GSC=""
	CONFIG_SND_HARMONY=""
	CONFIG_SND_SOC=""
	CONFIG_SND_SOC_AC97_BUS=""
	CONFIG_SND_AT32_SOC=""
	CONFIG_AVR32=""
	CONFIG_SND_AT32_SOC_SSC=""
	CONFIG_SND_AT32_SOC_PLAYPAQ=""
	CONFIG_BOARD_PLAYPAQ=""
	CONFIG_SND_AT32_SOC_PLAYPAQ_SLAVE=""
	CONFIG_SND_AT91_SOC=""
	CONFIG_ARCH_AT91=""
	CONFIG_SND_AT91_SOC_SSC=""
	CONFIG_SND_SOC_AU1XPSC=""
	CONFIG_SOC_AU1200=""
	CONFIG_SOC_AU1550=""
	CONFIG_SND_SOC_AU1XPSC_I2S=""
	CONFIG_SND_SOC_AU1XPSC_AC97=""
	CONFIG_SND_SOC_SAMPLE_PSC_AC97=""
	CONFIG_SND_PXA2XX_SOC=""
	CONFIG_SND_PXA2XX_SOC_AC97=""
	CONFIG_SND_PXA2XX_SOC_I2S=""
	CONFIG_SND_PXA2XX_SOC_CORGI=""
	CONFIG_PXA_SHARP_C7XX=""
	CONFIG_SND_PXA2XX_SOC_SPITZ=""
	CONFIG_PXA_SHARP_CXX00=""
	CONFIG_SND_PXA2XX_SOC_POODLE=""
	CONFIG_MACH_POODLE=""
	CONFIG_SND_PXA2XX_SOC_TOSA=""
	CONFIG_MACH_TOSA=""
	CONFIG_MFD_TC6393XB=""
	CONFIG_SND_PXA2XX_SOC_E800=""
	CONFIG_MACH_E800=""
	CONFIG_SND_PXA2XX_SOC_EM_X270=""
	CONFIG_MACH_EM_X270=""
	CONFIG_SND_S3C24XX_SOC=""
	CONFIG_SND_S3C24XX_SOC_I2S=""
	CONFIG_SND_S3C2412_SOC_I2S=""
	CONFIG_SND_S3C2443_SOC_AC97=""
	CONFIG_SND_S3C24XX_SOC_NEO1973_WM8753=""
	CONFIG_MACH_NEO1973_GTA01=""
	CONFIG_SND_S3C24XX_SOC_SMDK2443_WM9710=""
	CONFIG_MACH_SMDK2443=""
	CONFIG_SND_S3C24XX_SOC_LN2440SBC_ALC650=""
	CONFIG_SND_SOC_PCM_SH7760=""
	CONFIG_CPU_SUBTYPE_SH7760=""
	CONFIG_SH_DMABRG=""
	CONFIG_SND_SOC_SH4_HAC=""
	CONFIG_SND_SOC_SH4_SSI=""
	CONFIG_SND_SH7760_AC97=""
	CONFIG_SND_SOC_OF_SIMPLE=""
	CONFIG_SND_SOC_MPC8610=""
	CONFIG_MPC8610_HPCD=""
	CONFIG_SND_SOC_MPC8610_HPCD=""
	CONFIG_SND_SOC_MPC5200_I2S=""
	CONFIG_PPC_MPC52XX=""
	CONFIG_SND_DAVINCI_SOC=""
	CONFIG_ARCH_DAVINCI=""
	CONFIG_SND_DAVINCI_SOC_I2S=""
	CONFIG_SND_DAVINCI_SOC_EVM=""
	CONFIG_MACH_DAVINCI_EVM=""
	CONFIG_SND_OMAP_SOC=""
	CONFIG_ARCH_OMAP=""
	CONFIG_SND_OMAP_SOC_MCBSP=""
	CONFIG_OMAP_MCBSP=""
	CONFIG_SND_OMAP_SOC_N810=""
	CONFIG_MACH_NOKIA_N810=""
	CONFIG_SND_OMAP_SOC_OSK5912=""
	CONFIG_MACH_OMAP_OSK=""
	CONFIG_SND_BF5XX_I2S=""
	CONFIG_BLACKFIN=""
	CONFIG_SND_BF5XX_SOC_SSM2602=""
	CONFIG_I2C_BLACKFIN_TWI=""
	CONFIG_SND_BF5XX_SOC_AD73311=""
	CONFIG_SND_BFIN_AD73311_SE=""
	CONFIG_SND_BF5XX_AC97=""
	CONFIG_SND_MMAP_SUPPORT=""
	CONFIG_SND_BF5XX_SOC_SPORT=""
	CONFIG_SND_BF5XX_SOC_I2S=""
	CONFIG_SND_BF5XX_SOC_AC97=""
	CONFIG_SND_BF5XX_SOC_AD1980=""
	CONFIG_SND_BF5XX_SPORT_NUM=""
	CONFIG_SND_BF5XX_HAVE_COLD_RESET=""
	CONFIG_SND_BF5XX_RESET_GPIO_NUM=""
	CONFIG_SND_SOC_ALL_CODECS=""
	CONFIG_SPI_MASTER=""
	CONFIG_SND_SOC_AC97_CODEC=""
	CONFIG_SND_SOC_AD1980=""
	CONFIG_SND_SOC_AD73311=""
	CONFIG_SND_SOC_AK4535=""
	CONFIG_SND_SOC_CS4270=""
	CONFIG_SND_SOC_CS4270_HWMUTE=""
	CONFIG_SND_SOC_CS4270_VD33_ERRATA=""
	CONFIG_SND_SOC_SSM2602=""
	CONFIG_SND_SOC_TLV320AIC23=""
	CONFIG_SND_SOC_TLV320AIC26=""
	CONFIG_SND_SOC_TLV320AIC3X=""
	CONFIG_SND_SOC_UDA1380=""
	CONFIG_SND_SOC_WM8510=""
	CONFIG_SND_SOC_WM8580=""
	CONFIG_SND_SOC_WM8731=""
	CONFIG_SND_SOC_WM8750=""
	CONFIG_SND_SOC_WM8753=""
	CONFIG_SND_SOC_WM8900=""
	CONFIG_SND_SOC_WM8903=""
	CONFIG_SND_SOC_WM8971=""
	CONFIG_SND_SOC_WM8990=""
	CONFIG_SND_SOC_WM9712=""
	CONFIG_SND_SOC_WM9713=""
	CONFIG_SOUND_PRIME=""
	CONFIG_AC97_BUS=""
])

AC_DEFUN([ALSA_TOPLEVEL_SELECT], [
dnl Check for which cards to compile driver for...
AC_MSG_CHECKING(for cards to compile driver for)
AC_ARG_WITH(cards,
  [  --with-cards=<list> compile driver for cards and options in <list>; ]
  [                        cards may be separated with commas; ]
  [                        'all' compiles all drivers; ]
  [                        Possible cards are: ]
  [                          seq-dummy, rtctimer, hpet, pcsp, dummy, virmidi, ]
  [                          mtpav, mts64, serial-u16550, mpu401, portman2x4, ]
  [                          ml403-ac97cr, serialmidi, loopback, adlib, ]
  [                          ad1816a, ad1848, als100, azt2320, cmi8330, ]
  [                          cs4231, cs4232, cs4236, dt019x, es968, es1688, ]
  [                          es18xx, sc6000, gusclassic, gusextreme, gusmax, ]
  [                          interwave, interwave-stb, opl3sa2, ]
  [                          opti92x-ad1848, opti92x-cs4231, opti93x, miro, ]
  [                          sb8, sb16, sbawe, sgalaxy, sscape, wavefront, ]
  [                          pc98-cs4232, msnd-pinnacle, ad1889, als300, ]
  [                          als4000, ali5451, atiixp, atiixp-modem, au8810, ]
  [                          au8820, au8830, aw2, azt3328, bt87x, ca0106, ]
  [                          cmipci, oxygen, cs4281, cs46xx, cs5530, ]
  [                          cs5535audio, darla20, gina20, layla20, darla24, ]
  [                          gina24, layla24, mona, mia, echo3g, indigo, ]
  [                          indigoio, indigodj, emu10k1, emu10k1x, ens1370, ]
  [                          ens1371, es1938, es1968, fm801, hda-intel, hdsp, ]
  [                          hdspm, hifier, ice1712, ice1724, intel8x0, ]
  [                          intel8x0m, korg1212, maestro3, mixart, nm256, ]
  [                          pcxhr, riptide, rme32, rme96, rme9652, sis7019, ]
  [                          sonicvibes, trident, via82xx, via82xx-modem, ]
  [                          virtuoso, vx222, ymfpci, pdplus, asihpi, ]
  [                          powermac, ps3, aoa, aoa-fabric-layout, aoa-onyx, ]
  [                          aoa-tas, aoa-toonie, aoa-soundbus, ]
  [                          aoa-soundbus-i2s, sa11xx-uda1341, armaaci, ]
  [                          pxa2xx-ac97, s3c2410, at73c213, sgi-o2, sgi-hal2, ]
  [                          au1x00, aica, usb-audio, usb-usx2y, usb-caiaq, ]
  [                          usb-us122l, vxpocket, pdaudiocf, sun-amd7930, ]
  [                          sun-cs4231, sun-dbri, harmony, soc, at32-soc, ]
  [                          at32-soc-playpaq, at91-soc, soc-au1xpsc, ]
  [                          soc-sample-psc-ac97, pxa2xx-soc, ]
  [                          pxa2xx-soc-corgi, pxa2xx-soc-spitz, ]
  [                          pxa2xx-soc-poodle, pxa2xx-soc-tosa, ]
  [                          pxa2xx-soc-e800, pxa2xx-soc-em-x270, s3c24xx-soc, ]
  [                          s3c24xx-soc-neo1973-wm8753, ]
  [                          s3c24xx-soc-smdk2443-wm9710, ]
  [                          s3c24xx-soc-ln2440sbc-alc650, soc-pcm-sh7760, ]
  [                          sh7760-ac97, soc-mpc5200-i2s, davinci-soc, ]
  [                          davinci-soc-evm, omap-soc, omap-soc-n810, ]
  [                          omap-soc-osk5912, bf5xx-i2s, bf5xx-soc-ssm2602, ]
  [                          bf5xx-soc-ad73311, bf5xx-ac97, bf5xx-soc-ad1980, ]
  [                          soc-all-codecs, soc-tlv320aic26 ],
  cards="$withval", cards="all")
SELECTED_CARDS=`echo $cards | sed 's/,/ /g'`
AC_MSG_RESULT($SELECTED_CARDS)
AC_MSG_CHECKING(for additonal options to compile driver for)
AC_ARG_WITH(card_options,
  [  --with-card-options=<list> enable driver options in <list>; ]
  [                        options may be separated with commas; ]
  [                        'all' enables all options; ]
  [                        Possible options are: ]
  [                          seq-rtctimer-default, support-old-api, ]
  [                          verbose-procfs, verbose-printk, pcm-xrun-debug, ]
  [                          ac97-power-save, sb16-csp, ]
  [                          wavefront-firmware-in-kernel, bt87x-overclock, ]
  [                          cs46xx-new-dsp, fm801-tea575x-bool, hda-hwdep, ]
  [                          hda-input-beep, hda-codec-realtek, ]
  [                          hda-codec-analog, hda-codec-sigmatel, ]
  [                          hda-codec-via, hda-codec-atihdmi, ]
  [                          hda-codec-nvhdmi, hda-codec-conexant, ]
  [                          hda-codec-cmedia, hda-codec-si3054, hda-generic, ]
  [                          hda-power-save, powermac-auto-drc, ]
  [                          usb-caiaq-input, at32-soc-playpaq-slave, ]
  [                          soc-mpc8610, soc-mpc8610-hpcd, mmap-support, ]
  [                          bf5xx-have-cold-reset ],
  cards="$withval", cards="all")
SELECTED_OPTIONS=`echo $cards | sed 's/,/ /g'`
AC_MSG_RESULT($SELECTED_OPTIONS)
])
AC_DEFUN([ALSA_PARSE_KCONFIG], [
alsa_check_kconfig_card () {
  local pat=${1}
  for i in $SELECTED_CARDS; do
    case "$i" in
    $pat=n)
      return 1;;
    all|$pat|$pat=*)
      return 0;;
    esac
  done
  return 1
}
alsa_check_kconfig_option () {
  local pat=${1}
  for i in $SELECTED_OPTIONS; do
    case "$i" in
    $pat=n)
      return 1;;
    all|$pat|$pat=*)
      return 0;;
    esac
  done
  return 1
}
  CONFIG_SND="m"
  if alsa_check_kconfig_card "seq-dummy"; then
    if  ! ( test "$CONFIG_M68K" = "y" -o "$CONFIG_M68K" = "m" ) &&
      ( test "$CONFIG_SND_SEQUENCER" = "y" -o "$CONFIG_SND_SEQUENCER" = "m" ); then
      CONFIG_SND_SEQ_DUMMY="m"
    fi
  fi
  if alsa_check_kconfig_card "rtctimer"; then
    if  ! ( test "$CONFIG_M68K" = "y" -o "$CONFIG_M68K" = "m" ) &&
      ( test "$CONFIG_RTC" = "y" -o "$CONFIG_RTC" = "m" ) &&
      ( test "$kversion.$kpatchlevel" = "2.6" -a $ksublevel -ge 15 ); then
      CONFIG_SND_TIMER="m"
      CONFIG_SND_RTCTIMER="m"
    fi
  fi
  if alsa_check_kconfig_option "seq-rtctimer-default"; then
    if  ! ( test "$CONFIG_M68K" = "y" -o "$CONFIG_M68K" = "m" ) &&
      ( ( test "$CONFIG_SND_RTCTIMER" = "y" -o "$CONFIG_SND_RTCTIMER" = "m" ) &&
      ( test "$CONFIG_SND_SEQUENCER" = "y" -o "$CONFIG_SND_SEQUENCER" = "m" ) ); then
      CONFIG_SND_SEQ_RTCTIMER_DEFAULT="y"
    fi
  fi
  if alsa_check_kconfig_option "support-old-api"; then
    if  ! ( test "$CONFIG_M68K" = "y" -o "$CONFIG_M68K" = "m" ); then
      CONFIG_SND_SUPPORT_OLD_API="y"
    fi
  fi
  if alsa_check_kconfig_option "verbose-procfs"; then
    if  ! ( test "$CONFIG_M68K" = "y" -o "$CONFIG_M68K" = "m" ) &&
      ( test "$CONFIG_PROC_FS" = "y" -o "$CONFIG_PROC_FS" = "m" ); then
      CONFIG_SND_VERBOSE_PROCFS="y"
    fi
  fi
  if alsa_check_kconfig_option "verbose-printk"; then
    if  ! ( test "$CONFIG_M68K" = "y" -o "$CONFIG_M68K" = "m" ); then
      CONFIG_SND_VERBOSE_PRINTK="y"
    fi
  fi
  if alsa_check_kconfig_option "pcm-xrun-debug"; then
    if  ! ( test "$CONFIG_M68K" = "y" -o "$CONFIG_M68K" = "m" ) &&
      ( ( test "$CONFIG_SND_DEBUG" = "y" -o "$CONFIG_SND_DEBUG" = "m" ) &&
      ( test "$CONFIG_SND_VERBOSE_PROCFS" = "y" -o "$CONFIG_SND_VERBOSE_PROCFS" = "m" ) ); then
      CONFIG_SND_PCM_XRUN_DEBUG="y"
    fi
  fi
  if alsa_check_kconfig_card "hpet"; then
    if  ! ( test "$CONFIG_M68K" = "y" -o "$CONFIG_M68K" = "m" ) &&
      ( ( test "$CONFIG_HPET" = "y" -o "$CONFIG_HPET" = "m" ) &&
      ( test "$CONFIG_BROKEN" = "y" -o "$CONFIG_BROKEN" = "m" ) ); then
      CONFIG_SND_TIMER="m"
      CONFIG_SND_HPET="m"
    fi
  fi
    if  ! ( test "$CONFIG_M68K" = "y" -o "$CONFIG_M68K" = "m" ); then
      CONFIG_SND_DRIVERS="y"
    fi
  if alsa_check_kconfig_card "pcsp"; then
    if  ! ( test "$CONFIG_M68K" = "y" -o "$CONFIG_M68K" = "m" ) &&
      ( test "$CONFIG_SND_DRIVERS" = "y" -o "$CONFIG_SND_DRIVERS" = "m" ) &&
      ( ( test "$CONFIG_PCSPKR_PLATFORM" = "y" -o "$CONFIG_PCSPKR_PLATFORM" = "m" ) &&
      ( test "$CONFIG_X86_PC" = "y" -o "$CONFIG_X86_PC" = "m" ) &&
      ( test "$CONFIG_HIGH_RES_TIMERS" = "y" -o "$CONFIG_HIGH_RES_TIMERS" = "m" ) ) &&
      ( test "$CONFIG_INPUT" = "y" -o "$CONFIG_INPUT" = "m" ) &&
      ( test "$CONFIG_EXPERIMENTAL" = "y" -o "$CONFIG_EXPERIMENTAL" = "m" ) &&
      ( test "$kversion.$kpatchlevel" = "2.6" -a $ksublevel -ge 24 ); then
      CONFIG_SND_TIMER="m"
      CONFIG_SND_PCM="m"
      CONFIG_SND_PCSP="m"
    fi
  fi
  if alsa_check_kconfig_card "dummy"; then
    if  ! ( test "$CONFIG_M68K" = "y" -o "$CONFIG_M68K" = "m" ) &&
      ( test "$CONFIG_SND_DRIVERS" = "y" -o "$CONFIG_SND_DRIVERS" = "m" ); then
      CONFIG_SND_TIMER="m"
      CONFIG_SND_PCM="m"
      CONFIG_SND_DUMMY="m"
    fi
  fi
  if alsa_check_kconfig_card "virmidi"; then
    if  ! ( test "$CONFIG_M68K" = "y" -o "$CONFIG_M68K" = "m" ) &&
      ( test "$CONFIG_SND_DRIVERS" = "y" -o "$CONFIG_SND_DRIVERS" = "m" ) &&
      ( test "$CONFIG_SND_SEQUENCER" = "y" -o "$CONFIG_SND_SEQUENCER" = "m" ); then
      CONFIG_SND_TIMER="m"
      CONFIG_SND_RAWMIDI="m"
      CONFIG_SND_VIRMIDI="m"
    fi
  fi
  if alsa_check_kconfig_card "mtpav"; then
    if  ! ( test "$CONFIG_M68K" = "y" -o "$CONFIG_M68K" = "m" ) &&
      ( test "$CONFIG_SND_DRIVERS" = "y" -o "$CONFIG_SND_DRIVERS" = "m" ); then
      CONFIG_SND_RAWMIDI="m"
      CONFIG_SND_MTPAV="m"
    fi
  fi
  if alsa_check_kconfig_card "mts64"; then
    if  ! ( test "$CONFIG_M68K" = "y" -o "$CONFIG_M68K" = "m" ) &&
      ( test "$CONFIG_SND_DRIVERS" = "y" -o "$CONFIG_SND_DRIVERS" = "m" ) &&
      ( test "$CONFIG_PARPORT" = "y" -o "$CONFIG_PARPORT" = "m" ) &&
      ( test "$kversion.$kpatchlevel" = "2.6" -a $ksublevel -ge 10 ); then
      CONFIG_SND_RAWMIDI="m"
      CONFIG_SND_MTS64="m"
    fi
  fi
  if alsa_check_kconfig_card "serial-u16550"; then
    if  ! ( test "$CONFIG_M68K" = "y" -o "$CONFIG_M68K" = "m" ) &&
      ( test "$CONFIG_SND_DRIVERS" = "y" -o "$CONFIG_SND_DRIVERS" = "m" ); then
      CONFIG_SND_RAWMIDI="m"
      CONFIG_SND_SERIAL_U16550="m"
    fi
  fi
  if alsa_check_kconfig_card "mpu401"; then
    if  ! ( test "$CONFIG_M68K" = "y" -o "$CONFIG_M68K" = "m" ) &&
      ( test "$CONFIG_SND_DRIVERS" = "y" -o "$CONFIG_SND_DRIVERS" = "m" ); then
      CONFIG_SND_RAWMIDI="m"
      CONFIG_SND_MPU401_UART="m"
      CONFIG_SND_MPU401="m"
    fi
  fi
  if alsa_check_kconfig_card "portman2x4"; then
    if  ! ( test "$CONFIG_M68K" = "y" -o "$CONFIG_M68K" = "m" ) &&
      ( test "$CONFIG_SND_DRIVERS" = "y" -o "$CONFIG_SND_DRIVERS" = "m" ) &&
      ( test "$CONFIG_PARPORT" = "y" -o "$CONFIG_PARPORT" = "m" ) &&
      ( test "$kversion.$kpatchlevel" = "2.6" -a $ksublevel -ge 10 ); then
      CONFIG_SND_RAWMIDI="m"
      CONFIG_SND_PORTMAN2X4="m"
    fi
  fi
  if alsa_check_kconfig_card "ml403-ac97cr"; then
    if  ! ( test "$CONFIG_M68K" = "y" -o "$CONFIG_M68K" = "m" ) &&
      ( test "$CONFIG_SND_DRIVERS" = "y" -o "$CONFIG_SND_DRIVERS" = "m" ) &&
      ( test "$CONFIG_XILINX_VIRTEX" = "y" -o "$CONFIG_XILINX_VIRTEX" = "m" ); then
      CONFIG_SND_TIMER="m"
      CONFIG_SND_PCM="m"
      test "$kversion.$kpatchlevel" = "2.6" -a $ksublevel -ge 14 && CONFIG_AC97_BUS="m"
      CONFIG_SND_VMASTER="y"
      CONFIG_SND_AC97_CODEC="m"
      CONFIG_SND_ML403_AC97CR="m"
    fi
  fi
  if test "$CONFIG_CONFIG_SND_AC97_CODEC" = "m" -o "$CONFIG_CONFIG_SND_AC97_CODEC" = "y"; then
    if  ! ( test "$CONFIG_M68K" = "y" -o "$CONFIG_M68K" = "m" ); then
      CONFIG_SND_TIMER="m"
      CONFIG_SND_PCM="m"
      test "$kversion.$kpatchlevel" = "2.6" -a $ksublevel -ge 14 && CONFIG_AC97_BUS="m"
      CONFIG_SND_VMASTER="y"
    fi
  fi
  if alsa_check_kconfig_option "ac97-power-save"; then
    if  ! ( test "$CONFIG_M68K" = "y" -o "$CONFIG_M68K" = "m" ) &&
      ( test "$CONFIG_SND_DRIVERS" = "y" -o "$CONFIG_SND_DRIVERS" = "m" ) &&
      ( ( test "$CONFIG_SND_AC97_CODEC" = "y" -o "$CONFIG_SND_AC97_CODEC" = "m" ) &&
      ( test "$CONFIG_EXPERIMENTAL" = "y" -o "$CONFIG_EXPERIMENTAL" = "m" ) ); then
      CONFIG_SND_AC97_POWER_SAVE="y"
    fi
  fi
    if  ! ( test "$CONFIG_M68K" = "y" -o "$CONFIG_M68K" = "m" ) &&
      ( test "$CONFIG_SND_DRIVERS" = "y" -o "$CONFIG_SND_DRIVERS" = "m" ) &&
      ( test "$CONFIG_SND_AC97_POWER_SAVE" = "y" -o "$CONFIG_SND_AC97_POWER_SAVE" = "m" ); then
      CONFIG_SND_AC97_POWER_SAVE_DEFAULT="0"
    fi
  if alsa_check_kconfig_card "serialmidi"; then
    if  ! ( test "$CONFIG_M68K" = "y" -o "$CONFIG_M68K" = "m" ) &&
      ( test "$CONFIG_SND_DRIVERS" = "y" -o "$CONFIG_SND_DRIVERS" = "m" ) &&
      ( test "$CONFIG_BROKEN" = "y" -o "$CONFIG_BROKEN" = "m" ); then
      CONFIG_SND_RAWMIDI="m"
      CONFIG_SND_SERIALMIDI="m"
    fi
  fi
  if alsa_check_kconfig_card "loopback"; then
    if  ! ( test "$CONFIG_M68K" = "y" -o "$CONFIG_M68K" = "m" ) &&
      ( test "$CONFIG_SND_DRIVERS" = "y" -o "$CONFIG_SND_DRIVERS" = "m" ); then
      CONFIG_SND_TIMER="m"
      CONFIG_SND_PCM="m"
      CONFIG_SND_LOOPBACK="m"
    fi
  fi
    if  ! ( test "$CONFIG_M68K" = "y" -o "$CONFIG_M68K" = "m" ) &&
      ( ( test "$CONFIG_ISA" = "y" -o "$CONFIG_ISA" = "m" ) &&
      ( test "$CONFIG_ISA_DMA_API" = "y" -o "$CONFIG_ISA_DMA_API" = "m" ) ); then
      CONFIG_SND_ISA="y"
    fi
  if alsa_check_kconfig_card "adlib"; then
    if  ! ( test "$CONFIG_M68K" = "y" -o "$CONFIG_M68K" = "m" ) &&
      ( test "$CONFIG_SND_ISA" = "y" -o "$CONFIG_SND_ISA" = "m" ); then
      CONFIG_SND_TIMER="m"
      CONFIG_SND_HWDEP="m"
      CONFIG_SND_OPL3_LIB="m"
      CONFIG_SND_ADLIB="m"
    fi
  fi
  if alsa_check_kconfig_card "ad1816a"; then
    if  ! ( test "$CONFIG_M68K" = "y" -o "$CONFIG_M68K" = "m" ) &&
      ( test "$CONFIG_SND_ISA" = "y" -o "$CONFIG_SND_ISA" = "m" ) &&
      ( test "$CONFIG_PNP" = "y" -o "$CONFIG_PNP" = "m" ) &&
      ( test "$CONFIG_ISAPNP" = "y" -o "$CONFIG_ISAPNP" = "m" ); then
      CONFIG_SND_TIMER="m"
      CONFIG_SND_HWDEP="m"
      CONFIG_SND_RAWMIDI="m"
      CONFIG_SND_OPL3_LIB="m"
      CONFIG_SND_MPU401_UART="m"
      CONFIG_SND_PCM="m"
      CONFIG_SND_AD1816A="m"
    fi
  fi
  if alsa_check_kconfig_card "ad1848"; then
    if  ! ( test "$CONFIG_M68K" = "y" -o "$CONFIG_M68K" = "m" ) &&
      ( test "$CONFIG_SND_ISA" = "y" -o "$CONFIG_SND_ISA" = "m" ); then
      CONFIG_SND_TIMER="m"
      CONFIG_SND_PCM="m"
      CONFIG_SND_WSS_LIB="m"
      CONFIG_SND_AD1848="m"
    fi
  fi
  if alsa_check_kconfig_card "als100"; then
    if  ! ( test "$CONFIG_M68K" = "y" -o "$CONFIG_M68K" = "m" ) &&
      ( test "$CONFIG_SND_ISA" = "y" -o "$CONFIG_SND_ISA" = "m" ) &&
      ( test "$CONFIG_PNP" = "y" -o "$CONFIG_PNP" = "m" ) &&
      ( test "$CONFIG_ISAPNP" = "y" -o "$CONFIG_ISAPNP" = "m" ); then
      CONFIG_SND_TIMER="m"
      CONFIG_SND_HWDEP="m"
      CONFIG_SND_RAWMIDI="m"
      CONFIG_SND_PCM="m"
      CONFIG_SND_SB_COMMON="m"
      CONFIG_SND_OPL3_LIB="m"
      CONFIG_SND_MPU401_UART="m"
      CONFIG_SND_SB16_DSP="m"
      CONFIG_SND_ALS100="m"
    fi
  fi
  if alsa_check_kconfig_card "azt2320"; then
    if  ! ( test "$CONFIG_M68K" = "y" -o "$CONFIG_M68K" = "m" ) &&
      ( test "$CONFIG_SND_ISA" = "y" -o "$CONFIG_SND_ISA" = "m" ) &&
      ( test "$CONFIG_PNP" = "y" -o "$CONFIG_PNP" = "m" ) &&
      ( test "$CONFIG_ISAPNP" = "y" -o "$CONFIG_ISAPNP" = "m" ); then
      CONFIG_SND_TIMER="m"
      CONFIG_SND_HWDEP="m"
      CONFIG_SND_RAWMIDI="m"
      CONFIG_SND_PCM="m"
      CONFIG_SND_OPL3_LIB="m"
      CONFIG_SND_MPU401_UART="m"
      CONFIG_SND_WSS_LIB="m"
      CONFIG_SND_AZT2320="m"
    fi
  fi
  if alsa_check_kconfig_card "cmi8330"; then
    if  ! ( test "$CONFIG_M68K" = "y" -o "$CONFIG_M68K" = "m" ) &&
      ( test "$CONFIG_SND_ISA" = "y" -o "$CONFIG_SND_ISA" = "m" ); then
      CONFIG_SND_TIMER="m"
      CONFIG_SND_PCM="m"
      CONFIG_SND_SB_COMMON="m"
      CONFIG_SND_WSS_LIB="m"
      CONFIG_SND_SB16_DSP="m"
      CONFIG_SND_CMI8330="m"
    fi
  fi
  if alsa_check_kconfig_card "cs4231"; then
    if  ! ( test "$CONFIG_M68K" = "y" -o "$CONFIG_M68K" = "m" ) &&
      ( test "$CONFIG_SND_ISA" = "y" -o "$CONFIG_SND_ISA" = "m" ); then
      CONFIG_SND_RAWMIDI="m"
      CONFIG_SND_TIMER="m"
      CONFIG_SND_PCM="m"
      CONFIG_SND_MPU401_UART="m"
      CONFIG_SND_WSS_LIB="m"
      CONFIG_SND_CS4231="m"
    fi
  fi
  if alsa_check_kconfig_card "cs4232"; then
    if  ! ( test "$CONFIG_M68K" = "y" -o "$CONFIG_M68K" = "m" ) &&
      ( test "$CONFIG_SND_ISA" = "y" -o "$CONFIG_SND_ISA" = "m" ); then
      CONFIG_SND_TIMER="m"
      CONFIG_SND_HWDEP="m"
      CONFIG_SND_RAWMIDI="m"
      CONFIG_SND_PCM="m"
      CONFIG_SND_OPL3_LIB="m"
      CONFIG_SND_MPU401_UART="m"
      CONFIG_SND_WSS_LIB="m"
      CONFIG_SND_CS4232="m"
    fi
  fi
  if alsa_check_kconfig_card "cs4236"; then
    if  ! ( test "$CONFIG_M68K" = "y" -o "$CONFIG_M68K" = "m" ) &&
      ( test "$CONFIG_SND_ISA" = "y" -o "$CONFIG_SND_ISA" = "m" ); then
      CONFIG_SND_TIMER="m"
      CONFIG_SND_HWDEP="m"
      CONFIG_SND_RAWMIDI="m"
      CONFIG_SND_PCM="m"
      CONFIG_SND_OPL3_LIB="m"
      CONFIG_SND_MPU401_UART="m"
      CONFIG_SND_WSS_LIB="m"
      CONFIG_SND_CS4236="m"
    fi
  fi
  if alsa_check_kconfig_card "dt019x"; then
    if  ! ( test "$CONFIG_M68K" = "y" -o "$CONFIG_M68K" = "m" ) &&
      ( test "$CONFIG_SND_ISA" = "y" -o "$CONFIG_SND_ISA" = "m" ) &&
      ( test "$CONFIG_PNP" = "y" -o "$CONFIG_PNP" = "m" ) &&
      ( test "$CONFIG_ISAPNP" = "y" -o "$CONFIG_ISAPNP" = "m" ); then
      CONFIG_SND_TIMER="m"
      CONFIG_SND_HWDEP="m"
      CONFIG_SND_RAWMIDI="m"
      CONFIG_SND_PCM="m"
      CONFIG_SND_SB_COMMON="m"
      CONFIG_SND_OPL3_LIB="m"
      CONFIG_SND_MPU401_UART="m"
      CONFIG_SND_SB16_DSP="m"
      CONFIG_SND_DT019X="m"
    fi
  fi
  if alsa_check_kconfig_card "es968"; then
    if  ! ( test "$CONFIG_M68K" = "y" -o "$CONFIG_M68K" = "m" ) &&
      ( test "$CONFIG_SND_ISA" = "y" -o "$CONFIG_SND_ISA" = "m" ) &&
      ( test "$CONFIG_PNP" = "y" -o "$CONFIG_PNP" = "m" ) &&
      ( test "$CONFIG_ISAPNP" = "y" -o "$CONFIG_ISAPNP" = "m" ); then
      CONFIG_SND_RAWMIDI="m"
      CONFIG_SND_TIMER="m"
      CONFIG_SND_PCM="m"
      CONFIG_SND_SB_COMMON="m"
      CONFIG_SND_MPU401_UART="m"
      CONFIG_SND_SB8_DSP="m"
      CONFIG_SND_ES968="m"
    fi
  fi
  if alsa_check_kconfig_card "es1688"; then
    if  ! ( test "$CONFIG_M68K" = "y" -o "$CONFIG_M68K" = "m" ) &&
      ( test "$CONFIG_SND_ISA" = "y" -o "$CONFIG_SND_ISA" = "m" ); then
      CONFIG_SND_TIMER="m"
      CONFIG_SND_HWDEP="m"
      CONFIG_SND_RAWMIDI="m"
      CONFIG_SND_OPL3_LIB="m"
      CONFIG_SND_MPU401_UART="m"
      CONFIG_SND_PCM="m"
      CONFIG_SND_ES1688="m"
    fi
  fi
  if alsa_check_kconfig_card "es18xx"; then
    if  ! ( test "$CONFIG_M68K" = "y" -o "$CONFIG_M68K" = "m" ) &&
      ( test "$CONFIG_SND_ISA" = "y" -o "$CONFIG_SND_ISA" = "m" ); then
      CONFIG_SND_TIMER="m"
      CONFIG_SND_HWDEP="m"
      CONFIG_SND_RAWMIDI="m"
      CONFIG_SND_OPL3_LIB="m"
      CONFIG_SND_MPU401_UART="m"
      CONFIG_SND_PCM="m"
      CONFIG_SND_ES18XX="m"
    fi
  fi
  if alsa_check_kconfig_card "sc6000"; then
    if  ! ( test "$CONFIG_M68K" = "y" -o "$CONFIG_M68K" = "m" ) &&
      ( test "$CONFIG_SND_ISA" = "y" -o "$CONFIG_SND_ISA" = "m" ) &&
      ( test "$CONFIG_HAS_IOPORT" = "y" -o "$CONFIG_HAS_IOPORT" = "m" ); then
      CONFIG_SND_TIMER="m"
      CONFIG_SND_PCM="m"
      CONFIG_SND_HWDEP="m"
      CONFIG_SND_RAWMIDI="m"
      CONFIG_SND_WSS_LIB="m"
      CONFIG_SND_OPL3_LIB="m"
      CONFIG_SND_MPU401_UART="m"
      CONFIG_SND_SC6000="m"
    fi
  fi
  if alsa_check_kconfig_card "gusclassic"; then
    if  ! ( test "$CONFIG_M68K" = "y" -o "$CONFIG_M68K" = "m" ) &&
      ( test "$CONFIG_SND_ISA" = "y" -o "$CONFIG_SND_ISA" = "m" ); then
      CONFIG_SND_TIMER="m"
      CONFIG_SND_RAWMIDI="m"
      CONFIG_SND_PCM="m"
      CONFIG_SND_GUSCLASSIC="m"
    fi
  fi
  if alsa_check_kconfig_card "gusextreme"; then
    if  ! ( test "$CONFIG_M68K" = "y" -o "$CONFIG_M68K" = "m" ) &&
      ( test "$CONFIG_SND_ISA" = "y" -o "$CONFIG_SND_ISA" = "m" ); then
      CONFIG_SND_RAWMIDI="m"
      CONFIG_SND_TIMER="m"
      CONFIG_SND_HWDEP="m"
      CONFIG_SND_MPU401_UART="m"
      CONFIG_SND_PCM="m"
      CONFIG_SND_GUSEXTREME="m"
    fi
  fi
  if alsa_check_kconfig_card "gusmax"; then
    if  ! ( test "$CONFIG_M68K" = "y" -o "$CONFIG_M68K" = "m" ) &&
      ( test "$CONFIG_SND_ISA" = "y" -o "$CONFIG_SND_ISA" = "m" ); then
      CONFIG_SND_TIMER="m"
      CONFIG_SND_PCM="m"
      CONFIG_SND_RAWMIDI="m"
      CONFIG_SND_WSS_LIB="m"
      CONFIG_SND_GUSMAX="m"
    fi
  fi
  if alsa_check_kconfig_card "interwave"; then
    if  ! ( test "$CONFIG_M68K" = "y" -o "$CONFIG_M68K" = "m" ) &&
      ( test "$CONFIG_SND_ISA" = "y" -o "$CONFIG_SND_ISA" = "m" ) &&
      ( test "$CONFIG_PNP" = "y" -o "$CONFIG_PNP" = "m" ); then
      CONFIG_SND_TIMER="m"
      CONFIG_SND_PCM="m"
      CONFIG_SND_RAWMIDI="m"
      CONFIG_SND_WSS_LIB="m"
      CONFIG_SND_INTERWAVE="m"
    fi
  fi
  if alsa_check_kconfig_card "interwave-stb"; then
    if  ! ( test "$CONFIG_M68K" = "y" -o "$CONFIG_M68K" = "m" ) &&
      ( test "$CONFIG_SND_ISA" = "y" -o "$CONFIG_SND_ISA" = "m" ) &&
      ( test "$CONFIG_PNP" = "y" -o "$CONFIG_PNP" = "m" ); then
      CONFIG_SND_TIMER="m"
      CONFIG_SND_PCM="m"
      CONFIG_SND_RAWMIDI="m"
      CONFIG_SND_WSS_LIB="m"
      CONFIG_SND_INTERWAVE_STB="m"
    fi
  fi
  if alsa_check_kconfig_card "opl3sa2"; then
    if  ! ( test "$CONFIG_M68K" = "y" -o "$CONFIG_M68K" = "m" ) &&
      ( test "$CONFIG_SND_ISA" = "y" -o "$CONFIG_SND_ISA" = "m" ); then
      CONFIG_SND_TIMER="m"
      CONFIG_SND_HWDEP="m"
      CONFIG_SND_RAWMIDI="m"
      CONFIG_SND_PCM="m"
      CONFIG_SND_OPL3_LIB="m"
      CONFIG_SND_MPU401_UART="m"
      CONFIG_SND_WSS_LIB="m"
      CONFIG_SND_OPL3SA2="m"
    fi
  fi
  if alsa_check_kconfig_card "opti92x-ad1848"; then
    if  ! ( test "$CONFIG_M68K" = "y" -o "$CONFIG_M68K" = "m" ) &&
      ( test "$CONFIG_SND_ISA" = "y" -o "$CONFIG_SND_ISA" = "m" ); then
      CONFIG_SND_TIMER="m"
      CONFIG_SND_HWDEP="m"
      CONFIG_SND_RAWMIDI="m"
      CONFIG_SND_PCM="m"
      CONFIG_SND_OPL3_LIB="m"
      CONFIG_SND_OPL4_LIB="m"
      CONFIG_SND_MPU401_UART="m"
      CONFIG_SND_WSS_LIB="m"
      CONFIG_SND_OPTI92X_AD1848="m"
    fi
  fi
  if alsa_check_kconfig_card "opti92x-cs4231"; then
    if  ! ( test "$CONFIG_M68K" = "y" -o "$CONFIG_M68K" = "m" ) &&
      ( test "$CONFIG_SND_ISA" = "y" -o "$CONFIG_SND_ISA" = "m" ); then
      CONFIG_SND_TIMER="m"
      CONFIG_SND_HWDEP="m"
      CONFIG_SND_RAWMIDI="m"
      CONFIG_SND_PCM="m"
      CONFIG_SND_OPL3_LIB="m"
      CONFIG_SND_OPL4_LIB="m"
      CONFIG_SND_MPU401_UART="m"
      CONFIG_SND_WSS_LIB="m"
      CONFIG_SND_OPTI92X_CS4231="m"
    fi
  fi
  if alsa_check_kconfig_card "opti93x"; then
    if  ! ( test "$CONFIG_M68K" = "y" -o "$CONFIG_M68K" = "m" ) &&
      ( test "$CONFIG_SND_ISA" = "y" -o "$CONFIG_SND_ISA" = "m" ); then
      CONFIG_SND_TIMER="m"
      CONFIG_SND_HWDEP="m"
      CONFIG_SND_RAWMIDI="m"
      CONFIG_SND_PCM="m"
      CONFIG_SND_OPL3_LIB="m"
      CONFIG_SND_MPU401_UART="m"
      CONFIG_SND_WSS_LIB="m"
      CONFIG_SND_OPTI93X="m"
    fi
  fi
  if alsa_check_kconfig_card "miro"; then
    if  ! ( test "$CONFIG_M68K" = "y" -o "$CONFIG_M68K" = "m" ) &&
      ( test "$CONFIG_SND_ISA" = "y" -o "$CONFIG_SND_ISA" = "m" ); then
      CONFIG_SND_TIMER="m"
      CONFIG_SND_HWDEP="m"
      CONFIG_SND_PCM="m"
      CONFIG_SND_RAWMIDI="m"
      CONFIG_SND_OPL4_LIB="m"
      CONFIG_SND_WSS_LIB="m"
      CONFIG_SND_MPU401_UART="m"
      CONFIG_SND_PCM="m"
      CONFIG_SND_MIRO="m"
    fi
  fi
  if alsa_check_kconfig_card "sb8"; then
    if  ! ( test "$CONFIG_M68K" = "y" -o "$CONFIG_M68K" = "m" ) &&
      ( test "$CONFIG_SND_ISA" = "y" -o "$CONFIG_SND_ISA" = "m" ); then
      CONFIG_SND_TIMER="m"
      CONFIG_SND_HWDEP="m"
      CONFIG_SND_PCM="m"
      CONFIG_SND_SB_COMMON="m"
      CONFIG_SND_OPL3_LIB="m"
      CONFIG_SND_RAWMIDI="m"
      CONFIG_SND_SB8_DSP="m"
      CONFIG_SND_SB8="m"
    fi
  fi
  if alsa_check_kconfig_card "sb16"; then
    if  ! ( test "$CONFIG_M68K" = "y" -o "$CONFIG_M68K" = "m" ) &&
      ( test "$CONFIG_SND_ISA" = "y" -o "$CONFIG_SND_ISA" = "m" ); then
      CONFIG_SND_TIMER="m"
      CONFIG_SND_HWDEP="m"
      CONFIG_SND_RAWMIDI="m"
      CONFIG_SND_PCM="m"
      CONFIG_SND_SB_COMMON="m"
      CONFIG_SND_OPL3_LIB="m"
      CONFIG_SND_MPU401_UART="m"
      CONFIG_SND_SB16_DSP="m"
      CONFIG_SND_SB16="m"
    fi
  fi
  if alsa_check_kconfig_card "sbawe"; then
    if  ! ( test "$CONFIG_M68K" = "y" -o "$CONFIG_M68K" = "m" ) &&
      ( test "$CONFIG_SND_ISA" = "y" -o "$CONFIG_SND_ISA" = "m" ); then
      CONFIG_SND_TIMER="m"
      CONFIG_SND_HWDEP="m"
      CONFIG_SND_RAWMIDI="m"
      CONFIG_SND_PCM="m"
      CONFIG_SND_SB_COMMON="m"
      CONFIG_SND_OPL3_LIB="m"
      CONFIG_SND_MPU401_UART="m"
      CONFIG_SND_SB16_DSP="m"
      CONFIG_SND_SBAWE="m"
    fi
  fi
  if alsa_check_kconfig_option "sb16-csp"; then
    if  ! ( test "$CONFIG_M68K" = "y" -o "$CONFIG_M68K" = "m" ) &&
      ( test "$CONFIG_SND_ISA" = "y" -o "$CONFIG_SND_ISA" = "m" ) &&
      ( ( ( test "$CONFIG_SND_SB16" = "y" -o "$CONFIG_SND_SB16" = "m" ) ||
      ( test "$CONFIG_SND_SBAWE" = "y" -o "$CONFIG_SND_SBAWE" = "m" ) ) &&
      ( ( test "$CONFIG_BROKEN" = "y" -o "$CONFIG_BROKEN" = "m" ) ||
       ! ( test "$CONFIG_PPC" = "y" -o "$CONFIG_PPC" = "m" ) ) ) &&
      ( test "$CONFIG_FW_LOADER" = "y" -o "$CONFIG_FW_LOADER" = "m" ); then
      CONFIG_SND_SB16_CSP="y"
    fi
  fi
  if alsa_check_kconfig_card "sgalaxy"; then
    if  ! ( test "$CONFIG_M68K" = "y" -o "$CONFIG_M68K" = "m" ) &&
      ( test "$CONFIG_SND_ISA" = "y" -o "$CONFIG_SND_ISA" = "m" ); then
      CONFIG_SND_TIMER="m"
      CONFIG_SND_PCM="m"
      CONFIG_SND_WSS_LIB="m"
      CONFIG_SND_SGALAXY="m"
    fi
  fi
  if alsa_check_kconfig_card "sscape"; then
    if  ! ( test "$CONFIG_M68K" = "y" -o "$CONFIG_M68K" = "m" ) &&
      ( test "$CONFIG_SND_ISA" = "y" -o "$CONFIG_SND_ISA" = "m" ); then
      CONFIG_SND_RAWMIDI="m"
      CONFIG_SND_TIMER="m"
      CONFIG_SND_PCM="m"
      CONFIG_SND_HWDEP="m"
      CONFIG_SND_MPU401_UART="m"
      CONFIG_SND_WSS_LIB="m"
      CONFIG_SND_SSCAPE="m"
    fi
  fi
  if alsa_check_kconfig_card "wavefront"; then
    if  ! ( test "$CONFIG_M68K" = "y" -o "$CONFIG_M68K" = "m" ) &&
      ( test "$CONFIG_SND_ISA" = "y" -o "$CONFIG_SND_ISA" = "m" ) &&
      ( test "$CONFIG_FW_LOADER" = "y" -o "$CONFIG_FW_LOADER" = "m" ); then
      CONFIG_SND_TIMER="m"
      CONFIG_SND_HWDEP="m"
      CONFIG_SND_RAWMIDI="m"
      CONFIG_SND_PCM="m"
      CONFIG_SND_OPL3_LIB="m"
      CONFIG_SND_MPU401_UART="m"
      CONFIG_SND_WSS_LIB="m"
      CONFIG_SND_WAVEFRONT="m"
    fi
  fi
  if alsa_check_kconfig_option "wavefront-firmware-in-kernel"; then
    if  ! ( test "$CONFIG_M68K" = "y" -o "$CONFIG_M68K" = "m" ) &&
      ( test "$CONFIG_SND_ISA" = "y" -o "$CONFIG_SND_ISA" = "m" ) &&
      ( test "$CONFIG_SND_WAVEFRONT" = "y" -o "$CONFIG_SND_WAVEFRONT" = "m" ); then
      CONFIG_SND_WAVEFRONT_FIRMWARE_IN_KERNEL="y"
    fi
  fi
  if alsa_check_kconfig_card "pc98-cs4232"; then
    if  ! ( test "$CONFIG_M68K" = "y" -o "$CONFIG_M68K" = "m" ) &&
      ( test "$CONFIG_SND_ISA" = "y" -o "$CONFIG_SND_ISA" = "m" ) &&
      ( test "$CONFIG_X86_PC9800" = "y" -o "$CONFIG_X86_PC9800" = "m" ); then
      CONFIG_SND_TIMER="m"
      CONFIG_SND_HWDEP="m"
      CONFIG_SND_RAWMIDI="m"
      CONFIG_SND_OPL3_LIB="m"
      CONFIG_SND_MPU401_UART="m"
      CONFIG_SND_PCM="m"
      CONFIG_SND_PC98_CS4232="m"
    fi
  fi
  if alsa_check_kconfig_card "msnd-pinnacle"; then
    if  ! ( test "$CONFIG_M68K" = "y" -o "$CONFIG_M68K" = "m" ) &&
      ( test "$CONFIG_SND_ISA" = "y" -o "$CONFIG_SND_ISA" = "m" ) &&
      ( test "$CONFIG_X86" = "y" -o "$CONFIG_X86" = "m" ) &&
      ( test "$CONFIG_FW_LOADER" = "y" -o "$CONFIG_FW_LOADER" = "m" ); then
      CONFIG_SND_TIMER="m"
      CONFIG_SND_RAWMIDI="m"
      CONFIG_SND_PCM="m"
      CONFIG_SND_MSND_PINNACLE="m"
    fi
  fi
    if  ! ( test "$CONFIG_M68K" = "y" -o "$CONFIG_M68K" = "m" ) &&
      ( test "$CONFIG_PCI" = "y" -o "$CONFIG_PCI" = "m" ); then
      CONFIG_SND_PCI="y"
    fi
  if alsa_check_kconfig_card "ad1889"; then
    if  ! ( test "$CONFIG_M68K" = "y" -o "$CONFIG_M68K" = "m" ) &&
      ( test "$CONFIG_SND_PCI" = "y" -o "$CONFIG_SND_PCI" = "m" ); then
      CONFIG_SND_TIMER="m"
      CONFIG_SND_PCM="m"
      test "$kversion.$kpatchlevel" = "2.6" -a $ksublevel -ge 14 && CONFIG_AC97_BUS="m"
      CONFIG_SND_VMASTER="y"
      CONFIG_SND_AC97_CODEC="m"
      CONFIG_SND_AD1889="m"
    fi
  fi
  if alsa_check_kconfig_card "als300"; then
    if  ! ( test "$CONFIG_M68K" = "y" -o "$CONFIG_M68K" = "m" ) &&
      ( test "$CONFIG_SND_PCI" = "y" -o "$CONFIG_SND_PCI" = "m" ); then
      CONFIG_SND_TIMER="m"
      CONFIG_SND_PCM="m"
      test "$kversion.$kpatchlevel" = "2.6" -a $ksublevel -ge 14 && CONFIG_AC97_BUS="m"
      CONFIG_SND_VMASTER="y"
      CONFIG_SND_HWDEP="m"
      CONFIG_SND_PCM="m"
      CONFIG_SND_AC97_CODEC="m"
      CONFIG_SND_OPL3_LIB="m"
      CONFIG_SND_ALS300="m"
    fi
  fi
  if alsa_check_kconfig_card "als4000"; then
    if  ! ( test "$CONFIG_M68K" = "y" -o "$CONFIG_M68K" = "m" ) &&
      ( test "$CONFIG_SND_PCI" = "y" -o "$CONFIG_SND_PCI" = "m" ) &&
      ( test "$CONFIG_ISA_DMA_API" = "y" -o "$CONFIG_ISA_DMA_API" = "m" ); then
      CONFIG_SND_TIMER="m"
      CONFIG_SND_HWDEP="m"
      CONFIG_SND_RAWMIDI="m"
      CONFIG_SND_OPL3_LIB="m"
      CONFIG_SND_MPU401_UART="m"
      CONFIG_SND_PCM="m"
      CONFIG_SND_SB_COMMON="m"
      CONFIG_SND_ALS4000="m"
    fi
  fi
  if alsa_check_kconfig_card "ali5451"; then
    if  ! ( test "$CONFIG_M68K" = "y" -o "$CONFIG_M68K" = "m" ) &&
      ( test "$CONFIG_SND_PCI" = "y" -o "$CONFIG_SND_PCI" = "m" ); then
      CONFIG_SND_RAWMIDI="m"
      CONFIG_SND_TIMER="m"
      CONFIG_SND_PCM="m"
      test "$kversion.$kpatchlevel" = "2.6" -a $ksublevel -ge 14 && CONFIG_AC97_BUS="m"
      CONFIG_SND_VMASTER="y"
      CONFIG_SND_MPU401_UART="m"
      CONFIG_SND_AC97_CODEC="m"
      CONFIG_SND_ALI5451="m"
    fi
  fi
  if alsa_check_kconfig_card "atiixp"; then
    if  ! ( test "$CONFIG_M68K" = "y" -o "$CONFIG_M68K" = "m" ) &&
      ( test "$CONFIG_SND_PCI" = "y" -o "$CONFIG_SND_PCI" = "m" ); then
      CONFIG_SND_TIMER="m"
      CONFIG_SND_PCM="m"
      test "$kversion.$kpatchlevel" = "2.6" -a $ksublevel -ge 14 && CONFIG_AC97_BUS="m"
      CONFIG_SND_VMASTER="y"
      CONFIG_SND_AC97_CODEC="m"
      CONFIG_SND_ATIIXP="m"
    fi
  fi
  if alsa_check_kconfig_card "atiixp-modem"; then
    if  ! ( test "$CONFIG_M68K" = "y" -o "$CONFIG_M68K" = "m" ) &&
      ( test "$CONFIG_SND_PCI" = "y" -o "$CONFIG_SND_PCI" = "m" ); then
      CONFIG_SND_TIMER="m"
      CONFIG_SND_PCM="m"
      test "$kversion.$kpatchlevel" = "2.6" -a $ksublevel -ge 14 && CONFIG_AC97_BUS="m"
      CONFIG_SND_VMASTER="y"
      CONFIG_SND_AC97_CODEC="m"
      CONFIG_SND_ATIIXP_MODEM="m"
    fi
  fi
  if alsa_check_kconfig_card "au8810"; then
    if  ! ( test "$CONFIG_M68K" = "y" -o "$CONFIG_M68K" = "m" ) &&
      ( test "$CONFIG_SND_PCI" = "y" -o "$CONFIG_SND_PCI" = "m" ); then
      CONFIG_SND_RAWMIDI="m"
      CONFIG_SND_TIMER="m"
      CONFIG_SND_PCM="m"
      test "$kversion.$kpatchlevel" = "2.6" -a $ksublevel -ge 14 && CONFIG_AC97_BUS="m"
      CONFIG_SND_VMASTER="y"
      CONFIG_SND_MPU401_UART="m"
      CONFIG_SND_AC97_CODEC="m"
      CONFIG_SND_AU8810="m"
    fi
  fi
  if alsa_check_kconfig_card "au8820"; then
    if  ! ( test "$CONFIG_M68K" = "y" -o "$CONFIG_M68K" = "m" ) &&
      ( test "$CONFIG_SND_PCI" = "y" -o "$CONFIG_SND_PCI" = "m" ); then
      CONFIG_SND_RAWMIDI="m"
      CONFIG_SND_TIMER="m"
      CONFIG_SND_PCM="m"
      test "$kversion.$kpatchlevel" = "2.6" -a $ksublevel -ge 14 && CONFIG_AC97_BUS="m"
      CONFIG_SND_VMASTER="y"
      CONFIG_SND_MPU401_UART="m"
      CONFIG_SND_AC97_CODEC="m"
      CONFIG_SND_AU8820="m"
    fi
  fi
  if alsa_check_kconfig_card "au8830"; then
    if  ! ( test "$CONFIG_M68K" = "y" -o "$CONFIG_M68K" = "m" ) &&
      ( test "$CONFIG_SND_PCI" = "y" -o "$CONFIG_SND_PCI" = "m" ); then
      CONFIG_SND_RAWMIDI="m"
      CONFIG_SND_TIMER="m"
      CONFIG_SND_PCM="m"
      test "$kversion.$kpatchlevel" = "2.6" -a $ksublevel -ge 14 && CONFIG_AC97_BUS="m"
      CONFIG_SND_VMASTER="y"
      CONFIG_SND_MPU401_UART="m"
      CONFIG_SND_AC97_CODEC="m"
      CONFIG_SND_AU8830="m"
    fi
  fi
  if alsa_check_kconfig_card "aw2"; then
    if  ! ( test "$CONFIG_M68K" = "y" -o "$CONFIG_M68K" = "m" ) &&
      ( test "$CONFIG_SND_PCI" = "y" -o "$CONFIG_SND_PCI" = "m" ); then
      CONFIG_SND_AW2="m"
    fi
  fi
  if alsa_check_kconfig_card "azt3328"; then
    if  ! ( test "$CONFIG_M68K" = "y" -o "$CONFIG_M68K" = "m" ) &&
      ( test "$CONFIG_SND_PCI" = "y" -o "$CONFIG_SND_PCI" = "m" ) &&
      ( test "$CONFIG_EXPERIMENTAL" = "y" -o "$CONFIG_EXPERIMENTAL" = "m" ); then
      CONFIG_SND_TIMER="m"
      CONFIG_SND_HWDEP="m"
      CONFIG_SND_RAWMIDI="m"
      CONFIG_SND_OPL3_LIB="m"
      CONFIG_SND_MPU401_UART="m"
      CONFIG_SND_PCM="m"
      CONFIG_SND_AZT3328="m"
    fi
  fi
  if alsa_check_kconfig_card "bt87x"; then
    if  ! ( test "$CONFIG_M68K" = "y" -o "$CONFIG_M68K" = "m" ) &&
      ( test "$CONFIG_SND_PCI" = "y" -o "$CONFIG_SND_PCI" = "m" ); then
      CONFIG_SND_TIMER="m"
      CONFIG_SND_PCM="m"
      CONFIG_SND_BT87X="m"
    fi
  fi
  if alsa_check_kconfig_option "bt87x-overclock"; then
    if  ! ( test "$CONFIG_M68K" = "y" -o "$CONFIG_M68K" = "m" ) &&
      ( test "$CONFIG_SND_PCI" = "y" -o "$CONFIG_SND_PCI" = "m" ) &&
      ( test "$CONFIG_SND_BT87X" = "y" -o "$CONFIG_SND_BT87X" = "m" ); then
      CONFIG_SND_BT87X_OVERCLOCK="y"
    fi
  fi
  if alsa_check_kconfig_card "ca0106"; then
    if  ! ( test "$CONFIG_M68K" = "y" -o "$CONFIG_M68K" = "m" ) &&
      ( test "$CONFIG_SND_PCI" = "y" -o "$CONFIG_SND_PCI" = "m" ); then
      CONFIG_SND_TIMER="m"
      CONFIG_SND_PCM="m"
      test "$kversion.$kpatchlevel" = "2.6" -a $ksublevel -ge 14 && CONFIG_AC97_BUS="m"
      CONFIG_SND_VMASTER="y"
      CONFIG_SND_AC97_CODEC="m"
      CONFIG_SND_RAWMIDI="m"
      CONFIG_SND_VMASTER="y"
      CONFIG_SND_CA0106="m"
    fi
  fi
  if alsa_check_kconfig_card "cmipci"; then
    if  ! ( test "$CONFIG_M68K" = "y" -o "$CONFIG_M68K" = "m" ) &&
      ( test "$CONFIG_SND_PCI" = "y" -o "$CONFIG_SND_PCI" = "m" ); then
      CONFIG_SND_TIMER="m"
      CONFIG_SND_HWDEP="m"
      CONFIG_SND_RAWMIDI="m"
      CONFIG_SND_OPL3_LIB="m"
      CONFIG_SND_MPU401_UART="m"
      CONFIG_SND_PCM="m"
      CONFIG_SND_CMIPCI="m"
    fi
  fi
  if alsa_check_kconfig_card "oxygen"; then
    if  ! ( test "$CONFIG_M68K" = "y" -o "$CONFIG_M68K" = "m" ) &&
      ( test "$CONFIG_SND_PCI" = "y" -o "$CONFIG_SND_PCI" = "m" ); then
      CONFIG_SND_TIMER="m"
      CONFIG_SND_PCM="m"
      CONFIG_SND_RAWMIDI="m"
      CONFIG_SND_MPU401_UART="m"
      CONFIG_SND_OXYGEN_LIB="m"
      CONFIG_SND_OXYGEN="m"
    fi
  fi
  if alsa_check_kconfig_card "cs4281"; then
    if  ! ( test "$CONFIG_M68K" = "y" -o "$CONFIG_M68K" = "m" ) &&
      ( test "$CONFIG_SND_PCI" = "y" -o "$CONFIG_SND_PCI" = "m" ); then
      CONFIG_SND_TIMER="m"
      CONFIG_SND_HWDEP="m"
      CONFIG_SND_PCM="m"
      test "$kversion.$kpatchlevel" = "2.6" -a $ksublevel -ge 14 && CONFIG_AC97_BUS="m"
      CONFIG_SND_VMASTER="y"
      CONFIG_SND_OPL3_LIB="m"
      CONFIG_SND_RAWMIDI="m"
      CONFIG_SND_AC97_CODEC="m"
      CONFIG_SND_CS4281="m"
    fi
  fi
  if alsa_check_kconfig_card "cs46xx"; then
    if  ! ( test "$CONFIG_M68K" = "y" -o "$CONFIG_M68K" = "m" ) &&
      ( test "$CONFIG_SND_PCI" = "y" -o "$CONFIG_SND_PCI" = "m" ); then
      CONFIG_SND_TIMER="m"
      CONFIG_SND_PCM="m"
      test "$kversion.$kpatchlevel" = "2.6" -a $ksublevel -ge 14 && CONFIG_AC97_BUS="m"
      CONFIG_SND_VMASTER="y"
      CONFIG_SND_RAWMIDI="m"
      CONFIG_SND_AC97_CODEC="m"
      CONFIG_SND_CS46XX="m"
    fi
  fi
  if alsa_check_kconfig_option "cs46xx-new-dsp"; then
    if  ! ( test "$CONFIG_M68K" = "y" -o "$CONFIG_M68K" = "m" ) &&
      ( test "$CONFIG_SND_PCI" = "y" -o "$CONFIG_SND_PCI" = "m" ) &&
      ( test "$CONFIG_SND_CS46XX" = "y" -o "$CONFIG_SND_CS46XX" = "m" ); then
      CONFIG_SND_CS46XX_NEW_DSP="y"
    fi
  fi
  if alsa_check_kconfig_card "cs5530"; then
    if  ! ( test "$CONFIG_M68K" = "y" -o "$CONFIG_M68K" = "m" ) &&
      ( test "$CONFIG_SND_PCI" = "y" -o "$CONFIG_SND_PCI" = "m" ) &&
      ( test "$CONFIG_ISA_DMA_API" = "y" -o "$CONFIG_ISA_DMA_API" = "m" ); then
      CONFIG_SND_TIMER="m"
      CONFIG_SND_PCM="m"
      CONFIG_SND_SB_COMMON="m"
      CONFIG_SND_SB16_DSP="m"
      CONFIG_SND_CS5530="m"
    fi
  fi
  if alsa_check_kconfig_card "cs5535audio"; then
    if  ! ( test "$CONFIG_M68K" = "y" -o "$CONFIG_M68K" = "m" ) &&
      ( test "$CONFIG_SND_PCI" = "y" -o "$CONFIG_SND_PCI" = "m" ) &&
      ( ( test "$CONFIG_X86" = "y" -o "$CONFIG_X86" = "m" ) &&
       ! ( test "$CONFIG_X86_64" = "y" -o "$CONFIG_X86_64" = "m" ) ) &&
      ( test "$kversion.$kpatchlevel" = "2.6" -a $ksublevel -ge 10 ); then
      CONFIG_SND_TIMER="m"
      CONFIG_SND_PCM="m"
      test "$kversion.$kpatchlevel" = "2.6" -a $ksublevel -ge 14 && CONFIG_AC97_BUS="m"
      CONFIG_SND_VMASTER="y"
      CONFIG_SND_PCM="m"
      CONFIG_SND_AC97_CODEC="m"
      CONFIG_SND_CS5535AUDIO="m"
    fi
  fi
  if alsa_check_kconfig_card "darla20"; then
    if  ! ( test "$CONFIG_M68K" = "y" -o "$CONFIG_M68K" = "m" ) &&
      ( test "$CONFIG_SND_PCI" = "y" -o "$CONFIG_SND_PCI" = "m" ) &&
      ( test "$CONFIG_FW_LOADER" = "y" -o "$CONFIG_FW_LOADER" = "m" ); then
      CONFIG_SND_TIMER="m"
      CONFIG_SND_PCM="m"
      CONFIG_SND_DARLA20="m"
    fi
  fi
  if alsa_check_kconfig_card "gina20"; then
    if  ! ( test "$CONFIG_M68K" = "y" -o "$CONFIG_M68K" = "m" ) &&
      ( test "$CONFIG_SND_PCI" = "y" -o "$CONFIG_SND_PCI" = "m" ) &&
      ( test "$CONFIG_FW_LOADER" = "y" -o "$CONFIG_FW_LOADER" = "m" ); then
      CONFIG_SND_TIMER="m"
      CONFIG_SND_PCM="m"
      CONFIG_SND_GINA20="m"
    fi
  fi
  if alsa_check_kconfig_card "layla20"; then
    if  ! ( test "$CONFIG_M68K" = "y" -o "$CONFIG_M68K" = "m" ) &&
      ( test "$CONFIG_SND_PCI" = "y" -o "$CONFIG_SND_PCI" = "m" ) &&
      ( test "$CONFIG_FW_LOADER" = "y" -o "$CONFIG_FW_LOADER" = "m" ); then
      CONFIG_SND_TIMER="m"
      CONFIG_SND_RAWMIDI="m"
      CONFIG_SND_PCM="m"
      CONFIG_SND_LAYLA20="m"
    fi
  fi
  if alsa_check_kconfig_card "darla24"; then
    if  ! ( test "$CONFIG_M68K" = "y" -o "$CONFIG_M68K" = "m" ) &&
      ( test "$CONFIG_SND_PCI" = "y" -o "$CONFIG_SND_PCI" = "m" ) &&
      ( test "$CONFIG_FW_LOADER" = "y" -o "$CONFIG_FW_LOADER" = "m" ); then
      CONFIG_SND_TIMER="m"
      CONFIG_SND_PCM="m"
      CONFIG_SND_DARLA24="m"
    fi
  fi
  if alsa_check_kconfig_card "gina24"; then
    if  ! ( test "$CONFIG_M68K" = "y" -o "$CONFIG_M68K" = "m" ) &&
      ( test "$CONFIG_SND_PCI" = "y" -o "$CONFIG_SND_PCI" = "m" ) &&
      ( test "$CONFIG_FW_LOADER" = "y" -o "$CONFIG_FW_LOADER" = "m" ); then
      CONFIG_SND_TIMER="m"
      CONFIG_SND_PCM="m"
      CONFIG_SND_GINA24="m"
    fi
  fi
  if alsa_check_kconfig_card "layla24"; then
    if  ! ( test "$CONFIG_M68K" = "y" -o "$CONFIG_M68K" = "m" ) &&
      ( test "$CONFIG_SND_PCI" = "y" -o "$CONFIG_SND_PCI" = "m" ) &&
      ( test "$CONFIG_FW_LOADER" = "y" -o "$CONFIG_FW_LOADER" = "m" ); then
      CONFIG_SND_TIMER="m"
      CONFIG_SND_RAWMIDI="m"
      CONFIG_SND_PCM="m"
      CONFIG_SND_LAYLA24="m"
    fi
  fi
  if alsa_check_kconfig_card "mona"; then
    if  ! ( test "$CONFIG_M68K" = "y" -o "$CONFIG_M68K" = "m" ) &&
      ( test "$CONFIG_SND_PCI" = "y" -o "$CONFIG_SND_PCI" = "m" ) &&
      ( test "$CONFIG_FW_LOADER" = "y" -o "$CONFIG_FW_LOADER" = "m" ); then
      CONFIG_SND_TIMER="m"
      CONFIG_SND_RAWMIDI="m"
      CONFIG_SND_PCM="m"
      CONFIG_SND_MONA="m"
    fi
  fi
  if alsa_check_kconfig_card "mia"; then
    if  ! ( test "$CONFIG_M68K" = "y" -o "$CONFIG_M68K" = "m" ) &&
      ( test "$CONFIG_SND_PCI" = "y" -o "$CONFIG_SND_PCI" = "m" ) &&
      ( test "$CONFIG_FW_LOADER" = "y" -o "$CONFIG_FW_LOADER" = "m" ); then
      CONFIG_SND_TIMER="m"
      CONFIG_SND_RAWMIDI="m"
      CONFIG_SND_PCM="m"
      CONFIG_SND_MIA="m"
    fi
  fi
  if alsa_check_kconfig_card "echo3g"; then
    if  ! ( test "$CONFIG_M68K" = "y" -o "$CONFIG_M68K" = "m" ) &&
      ( test "$CONFIG_SND_PCI" = "y" -o "$CONFIG_SND_PCI" = "m" ) &&
      ( test "$CONFIG_FW_LOADER" = "y" -o "$CONFIG_FW_LOADER" = "m" ); then
      CONFIG_SND_TIMER="m"
      CONFIG_SND_RAWMIDI="m"
      CONFIG_SND_PCM="m"
      CONFIG_SND_ECHO3G="m"
    fi
  fi
  if alsa_check_kconfig_card "indigo"; then
    if  ! ( test "$CONFIG_M68K" = "y" -o "$CONFIG_M68K" = "m" ) &&
      ( test "$CONFIG_SND_PCI" = "y" -o "$CONFIG_SND_PCI" = "m" ) &&
      ( test "$CONFIG_FW_LOADER" = "y" -o "$CONFIG_FW_LOADER" = "m" ); then
      CONFIG_SND_TIMER="m"
      CONFIG_SND_PCM="m"
      CONFIG_SND_INDIGO="m"
    fi
  fi
  if alsa_check_kconfig_card "indigoio"; then
    if  ! ( test "$CONFIG_M68K" = "y" -o "$CONFIG_M68K" = "m" ) &&
      ( test "$CONFIG_SND_PCI" = "y" -o "$CONFIG_SND_PCI" = "m" ) &&
      ( test "$CONFIG_FW_LOADER" = "y" -o "$CONFIG_FW_LOADER" = "m" ); then
      CONFIG_SND_TIMER="m"
      CONFIG_SND_PCM="m"
      CONFIG_SND_INDIGOIO="m"
    fi
  fi
  if alsa_check_kconfig_card "indigodj"; then
    if  ! ( test "$CONFIG_M68K" = "y" -o "$CONFIG_M68K" = "m" ) &&
      ( test "$CONFIG_SND_PCI" = "y" -o "$CONFIG_SND_PCI" = "m" ) &&
      ( test "$CONFIG_FW_LOADER" = "y" -o "$CONFIG_FW_LOADER" = "m" ); then
      CONFIG_SND_TIMER="m"
      CONFIG_SND_PCM="m"
      CONFIG_SND_INDIGODJ="m"
    fi
  fi
  if alsa_check_kconfig_card "emu10k1"; then
    if  ! ( test "$CONFIG_M68K" = "y" -o "$CONFIG_M68K" = "m" ) &&
      ( test "$CONFIG_SND_PCI" = "y" -o "$CONFIG_SND_PCI" = "m" ) &&
      ( test "$CONFIG_FW_LOADER" = "y" -o "$CONFIG_FW_LOADER" = "m" ); then
      CONFIG_SND_TIMER="m"
      CONFIG_SND_PCM="m"
      test "$kversion.$kpatchlevel" = "2.6" -a $ksublevel -ge 14 && CONFIG_AC97_BUS="m"
      CONFIG_SND_VMASTER="y"
      CONFIG_SND_HWDEP="m"
      CONFIG_SND_RAWMIDI="m"
      CONFIG_SND_AC97_CODEC="m"
      CONFIG_SND_EMU10K1="m"
    fi
  fi
  if alsa_check_kconfig_card "emu10k1x"; then
    if  ! ( test "$CONFIG_M68K" = "y" -o "$CONFIG_M68K" = "m" ) &&
      ( test "$CONFIG_SND_PCI" = "y" -o "$CONFIG_SND_PCI" = "m" ); then
      CONFIG_SND_TIMER="m"
      CONFIG_SND_PCM="m"
      test "$kversion.$kpatchlevel" = "2.6" -a $ksublevel -ge 14 && CONFIG_AC97_BUS="m"
      CONFIG_SND_VMASTER="y"
      CONFIG_SND_AC97_CODEC="m"
      CONFIG_SND_RAWMIDI="m"
      CONFIG_SND_EMU10K1X="m"
    fi
  fi
  if alsa_check_kconfig_card "ens1370"; then
    if  ! ( test "$CONFIG_M68K" = "y" -o "$CONFIG_M68K" = "m" ) &&
      ( test "$CONFIG_SND_PCI" = "y" -o "$CONFIG_SND_PCI" = "m" ); then
      CONFIG_SND_TIMER="m"
      CONFIG_SND_RAWMIDI="m"
      CONFIG_SND_PCM="m"
      CONFIG_SND_ENS1370="m"
    fi
  fi
  if alsa_check_kconfig_card "ens1371"; then
    if  ! ( test "$CONFIG_M68K" = "y" -o "$CONFIG_M68K" = "m" ) &&
      ( test "$CONFIG_SND_PCI" = "y" -o "$CONFIG_SND_PCI" = "m" ); then
      CONFIG_SND_TIMER="m"
      CONFIG_SND_PCM="m"
      test "$kversion.$kpatchlevel" = "2.6" -a $ksublevel -ge 14 && CONFIG_AC97_BUS="m"
      CONFIG_SND_VMASTER="y"
      CONFIG_SND_RAWMIDI="m"
      CONFIG_SND_AC97_CODEC="m"
      CONFIG_SND_ENS1371="m"
    fi
  fi
  if alsa_check_kconfig_card "es1938"; then
    if  ! ( test "$CONFIG_M68K" = "y" -o "$CONFIG_M68K" = "m" ) &&
      ( test "$CONFIG_SND_PCI" = "y" -o "$CONFIG_SND_PCI" = "m" ); then
      CONFIG_SND_TIMER="m"
      CONFIG_SND_HWDEP="m"
      CONFIG_SND_RAWMIDI="m"
      CONFIG_SND_PCM="m"
      test "$kversion.$kpatchlevel" = "2.6" -a $ksublevel -ge 14 && CONFIG_AC97_BUS="m"
      CONFIG_SND_VMASTER="y"
      CONFIG_SND_OPL3_LIB="m"
      CONFIG_SND_MPU401_UART="m"
      CONFIG_SND_AC97_CODEC="m"
      CONFIG_SND_ES1938="m"
    fi
  fi
  if alsa_check_kconfig_card "es1968"; then
    if  ! ( test "$CONFIG_M68K" = "y" -o "$CONFIG_M68K" = "m" ) &&
      ( test "$CONFIG_SND_PCI" = "y" -o "$CONFIG_SND_PCI" = "m" ); then
      CONFIG_SND_RAWMIDI="m"
      CONFIG_SND_TIMER="m"
      CONFIG_SND_PCM="m"
      test "$kversion.$kpatchlevel" = "2.6" -a $ksublevel -ge 14 && CONFIG_AC97_BUS="m"
      CONFIG_SND_VMASTER="y"
      CONFIG_SND_MPU401_UART="m"
      CONFIG_SND_AC97_CODEC="m"
      CONFIG_SND_ES1968="m"
    fi
  fi
  if alsa_check_kconfig_card "fm801"; then
    if  ! ( test "$CONFIG_M68K" = "y" -o "$CONFIG_M68K" = "m" ) &&
      ( test "$CONFIG_SND_PCI" = "y" -o "$CONFIG_SND_PCI" = "m" ); then
      CONFIG_SND_TIMER="m"
      CONFIG_SND_HWDEP="m"
      CONFIG_SND_RAWMIDI="m"
      CONFIG_SND_PCM="m"
      test "$kversion.$kpatchlevel" = "2.6" -a $ksublevel -ge 14 && CONFIG_AC97_BUS="m"
      CONFIG_SND_VMASTER="y"
      CONFIG_SND_OPL3_LIB="m"
      CONFIG_SND_MPU401_UART="m"
      CONFIG_SND_AC97_CODEC="m"
      CONFIG_SND_FM801="m"
    fi
  fi
  if alsa_check_kconfig_option "fm801-tea575x-bool"; then
    if  ! ( test "$CONFIG_M68K" = "y" -o "$CONFIG_M68K" = "m" ) &&
      ( test "$CONFIG_SND_PCI" = "y" -o "$CONFIG_SND_PCI" = "m" ) &&
      ( test "$CONFIG_SND_FM801" = "y" -o "$CONFIG_SND_FM801" = "m" ) &&
      ( test "$CONFIG_VIDEO_V4L1" = "y" -o "$CONFIG_VIDEO_V4L1" = "m" ); then
      CONFIG_SND_FM801_TEA575X_BOOL="y"
    fi
  fi
  if alsa_check_kconfig_card "hda-intel"; then
    if  ! ( test "$CONFIG_M68K" = "y" -o "$CONFIG_M68K" = "m" ) &&
      ( test "$CONFIG_SND_PCI" = "y" -o "$CONFIG_SND_PCI" = "m" ) &&
      ( test "$kversion.$kpatchlevel" = "2.6" ); then
      CONFIG_SND_TIMER="m"
      CONFIG_SND_PCM="m"
      CONFIG_SND_VMASTER="y"
      test "$kversion.$kpatchlevel" = "2.6" -a $ksublevel -ge 27 && CONFIG_SND_JACK="y"
      CONFIG_SND_HDA_INTEL="m"
    fi
  fi
  if alsa_check_kconfig_option "hda-hwdep"; then
    if  ! ( test "$CONFIG_M68K" = "y" -o "$CONFIG_M68K" = "m" ) &&
      ( test "$CONFIG_SND_PCI" = "y" -o "$CONFIG_SND_PCI" = "m" ) &&
      ( test "$CONFIG_SND_HDA_INTEL" = "y" -o "$CONFIG_SND_HDA_INTEL" = "m" ); then
      CONFIG_SND_HWDEP="m"
      CONFIG_SND_HDA_HWDEP="y"
    fi
  fi
  if alsa_check_kconfig_option "hda-input-beep"; then
    if  ! ( test "$CONFIG_M68K" = "y" -o "$CONFIG_M68K" = "m" ) &&
      ( test "$CONFIG_SND_PCI" = "y" -o "$CONFIG_SND_PCI" = "m" ) &&
      ( test "$CONFIG_SND_HDA_INTEL" = "y" -o "$CONFIG_SND_HDA_INTEL" = "m" ) &&
      ( test "$CONFIG_INPUT" = "y" -o "$CONFIG_INPUT" = "m" ) &&
      ( test "$kversion.$kpatchlevel" = "2.6" ); then
      CONFIG_SND_HDA_INPUT_BEEP="y"
    fi
  fi
  if alsa_check_kconfig_option "hda-codec-realtek"; then
    if  ! ( test "$CONFIG_M68K" = "y" -o "$CONFIG_M68K" = "m" ) &&
      ( test "$CONFIG_SND_PCI" = "y" -o "$CONFIG_SND_PCI" = "m" ) &&
      ( test "$CONFIG_SND_HDA_INTEL" = "y" -o "$CONFIG_SND_HDA_INTEL" = "m" ); then
      CONFIG_SND_HDA_CODEC_REALTEK="y"
    fi
  fi
  if alsa_check_kconfig_option "hda-codec-analog"; then
    if  ! ( test "$CONFIG_M68K" = "y" -o "$CONFIG_M68K" = "m" ) &&
      ( test "$CONFIG_SND_PCI" = "y" -o "$CONFIG_SND_PCI" = "m" ) &&
      ( test "$CONFIG_SND_HDA_INTEL" = "y" -o "$CONFIG_SND_HDA_INTEL" = "m" ); then
      CONFIG_SND_HDA_CODEC_ANALOG="y"
    fi
  fi
  if alsa_check_kconfig_option "hda-codec-sigmatel"; then
    if  ! ( test "$CONFIG_M68K" = "y" -o "$CONFIG_M68K" = "m" ) &&
      ( test "$CONFIG_SND_PCI" = "y" -o "$CONFIG_SND_PCI" = "m" ) &&
      ( test "$CONFIG_SND_HDA_INTEL" = "y" -o "$CONFIG_SND_HDA_INTEL" = "m" ); then
      CONFIG_SND_HDA_CODEC_SIGMATEL="y"
    fi
  fi
  if alsa_check_kconfig_option "hda-codec-via"; then
    if  ! ( test "$CONFIG_M68K" = "y" -o "$CONFIG_M68K" = "m" ) &&
      ( test "$CONFIG_SND_PCI" = "y" -o "$CONFIG_SND_PCI" = "m" ) &&
      ( test "$CONFIG_SND_HDA_INTEL" = "y" -o "$CONFIG_SND_HDA_INTEL" = "m" ); then
      CONFIG_SND_HDA_CODEC_VIA="y"
    fi
  fi
  if alsa_check_kconfig_option "hda-codec-atihdmi"; then
    if  ! ( test "$CONFIG_M68K" = "y" -o "$CONFIG_M68K" = "m" ) &&
      ( test "$CONFIG_SND_PCI" = "y" -o "$CONFIG_SND_PCI" = "m" ) &&
      ( test "$CONFIG_SND_HDA_INTEL" = "y" -o "$CONFIG_SND_HDA_INTEL" = "m" ); then
      CONFIG_SND_HDA_CODEC_ATIHDMI="y"
    fi
  fi
  if alsa_check_kconfig_option "hda-codec-nvhdmi"; then
    if  ! ( test "$CONFIG_M68K" = "y" -o "$CONFIG_M68K" = "m" ) &&
      ( test "$CONFIG_SND_PCI" = "y" -o "$CONFIG_SND_PCI" = "m" ) &&
      ( test "$CONFIG_SND_HDA_INTEL" = "y" -o "$CONFIG_SND_HDA_INTEL" = "m" ); then
      CONFIG_SND_HDA_CODEC_NVHDMI="y"
    fi
  fi
  if alsa_check_kconfig_option "hda-codec-conexant"; then
    if  ! ( test "$CONFIG_M68K" = "y" -o "$CONFIG_M68K" = "m" ) &&
      ( test "$CONFIG_SND_PCI" = "y" -o "$CONFIG_SND_PCI" = "m" ) &&
      ( test "$CONFIG_SND_HDA_INTEL" = "y" -o "$CONFIG_SND_HDA_INTEL" = "m" ); then
      CONFIG_SND_HDA_CODEC_CONEXANT="y"
    fi
  fi
  if alsa_check_kconfig_option "hda-codec-cmedia"; then
    if  ! ( test "$CONFIG_M68K" = "y" -o "$CONFIG_M68K" = "m" ) &&
      ( test "$CONFIG_SND_PCI" = "y" -o "$CONFIG_SND_PCI" = "m" ) &&
      ( test "$CONFIG_SND_HDA_INTEL" = "y" -o "$CONFIG_SND_HDA_INTEL" = "m" ); then
      CONFIG_SND_HDA_CODEC_CMEDIA="y"
    fi
  fi
  if alsa_check_kconfig_option "hda-codec-si3054"; then
    if  ! ( test "$CONFIG_M68K" = "y" -o "$CONFIG_M68K" = "m" ) &&
      ( test "$CONFIG_SND_PCI" = "y" -o "$CONFIG_SND_PCI" = "m" ) &&
      ( test "$CONFIG_SND_HDA_INTEL" = "y" -o "$CONFIG_SND_HDA_INTEL" = "m" ); then
      CONFIG_SND_HDA_CODEC_SI3054="y"
    fi
  fi
  if alsa_check_kconfig_option "hda-generic"; then
    if  ! ( test "$CONFIG_M68K" = "y" -o "$CONFIG_M68K" = "m" ) &&
      ( test "$CONFIG_SND_PCI" = "y" -o "$CONFIG_SND_PCI" = "m" ) &&
      ( test "$CONFIG_SND_HDA_INTEL" = "y" -o "$CONFIG_SND_HDA_INTEL" = "m" ); then
      CONFIG_SND_HDA_GENERIC="y"
    fi
  fi
  if alsa_check_kconfig_option "hda-power-save"; then
    if  ! ( test "$CONFIG_M68K" = "y" -o "$CONFIG_M68K" = "m" ) &&
      ( test "$CONFIG_SND_PCI" = "y" -o "$CONFIG_SND_PCI" = "m" ) &&
      ( ( test "$CONFIG_SND_HDA_INTEL" = "y" -o "$CONFIG_SND_HDA_INTEL" = "m" ) &&
      ( test "$CONFIG_EXPERIMENTAL" = "y" -o "$CONFIG_EXPERIMENTAL" = "m" ) ); then
      CONFIG_SND_HDA_POWER_SAVE="y"
    fi
  fi
    if  ! ( test "$CONFIG_M68K" = "y" -o "$CONFIG_M68K" = "m" ) &&
      ( test "$CONFIG_SND_PCI" = "y" -o "$CONFIG_SND_PCI" = "m" ) &&
      ( test "$CONFIG_SND_HDA_POWER_SAVE" = "y" -o "$CONFIG_SND_HDA_POWER_SAVE" = "m" ); then
      CONFIG_SND_HDA_POWER_SAVE_DEFAULT="0"
    fi
  if alsa_check_kconfig_card "hdsp"; then
    if  ! ( test "$CONFIG_M68K" = "y" -o "$CONFIG_M68K" = "m" ) &&
      ( test "$CONFIG_SND_PCI" = "y" -o "$CONFIG_SND_PCI" = "m" ); then
      CONFIG_SND_TIMER="m"
      CONFIG_SND_HWDEP="m"
      CONFIG_SND_RAWMIDI="m"
      CONFIG_SND_PCM="m"
      CONFIG_SND_HDSP="m"
    fi
  fi
  if alsa_check_kconfig_card "hdspm"; then
    if  ! ( test "$CONFIG_M68K" = "y" -o "$CONFIG_M68K" = "m" ) &&
      ( test "$CONFIG_SND_PCI" = "y" -o "$CONFIG_SND_PCI" = "m" ); then
      CONFIG_SND_TIMER="m"
      CONFIG_SND_HWDEP="m"
      CONFIG_SND_RAWMIDI="m"
      CONFIG_SND_PCM="m"
      CONFIG_SND_HDSPM="m"
    fi
  fi
  if alsa_check_kconfig_card "hifier"; then
    if  ! ( test "$CONFIG_M68K" = "y" -o "$CONFIG_M68K" = "m" ) &&
      ( test "$CONFIG_SND_PCI" = "y" -o "$CONFIG_SND_PCI" = "m" ); then
      CONFIG_SND_TIMER="m"
      CONFIG_SND_PCM="m"
      CONFIG_SND_RAWMIDI="m"
      CONFIG_SND_MPU401_UART="m"
      CONFIG_SND_OXYGEN_LIB="m"
      CONFIG_SND_HIFIER="m"
    fi
  fi
  if alsa_check_kconfig_card "ice1712"; then
    if  ! ( test "$CONFIG_M68K" = "y" -o "$CONFIG_M68K" = "m" ) &&
      ( test "$CONFIG_SND_PCI" = "y" -o "$CONFIG_SND_PCI" = "m" ); then
      CONFIG_SND_RAWMIDI="m"
      CONFIG_SND_TIMER="m"
      CONFIG_SND_PCM="m"
      test "$kversion.$kpatchlevel" = "2.6" -a $ksublevel -ge 14 && CONFIG_AC97_BUS="m"
      CONFIG_SND_VMASTER="y"
      CONFIG_SND_MPU401_UART="m"
      CONFIG_SND_AC97_CODEC="m"
      CONFIG_SND_ICE1712="m"
    fi
  fi
  if alsa_check_kconfig_card "ice1724"; then
    if  ! ( test "$CONFIG_M68K" = "y" -o "$CONFIG_M68K" = "m" ) &&
      ( test "$CONFIG_SND_PCI" = "y" -o "$CONFIG_SND_PCI" = "m" ); then
      CONFIG_SND_TIMER="m"
      CONFIG_SND_PCM="m"
      test "$kversion.$kpatchlevel" = "2.6" -a $ksublevel -ge 14 && CONFIG_AC97_BUS="m"
      CONFIG_SND_VMASTER="y"
      CONFIG_SND_RAWMIDI="m"
      CONFIG_SND_AC97_CODEC="m"
      CONFIG_SND_VMASTER="y"
      CONFIG_SND_ICE1724="m"
    fi
  fi
  if alsa_check_kconfig_card "intel8x0"; then
    if  ! ( test "$CONFIG_M68K" = "y" -o "$CONFIG_M68K" = "m" ) &&
      ( test "$CONFIG_SND_PCI" = "y" -o "$CONFIG_SND_PCI" = "m" ); then
      CONFIG_SND_TIMER="m"
      CONFIG_SND_PCM="m"
      test "$kversion.$kpatchlevel" = "2.6" -a $ksublevel -ge 14 && CONFIG_AC97_BUS="m"
      CONFIG_SND_VMASTER="y"
      CONFIG_SND_AC97_CODEC="m"
      CONFIG_SND_INTEL8X0="m"
    fi
  fi
  if alsa_check_kconfig_card "intel8x0m"; then
    if  ! ( test "$CONFIG_M68K" = "y" -o "$CONFIG_M68K" = "m" ) &&
      ( test "$CONFIG_SND_PCI" = "y" -o "$CONFIG_SND_PCI" = "m" ); then
      CONFIG_SND_TIMER="m"
      CONFIG_SND_PCM="m"
      test "$kversion.$kpatchlevel" = "2.6" -a $ksublevel -ge 14 && CONFIG_AC97_BUS="m"
      CONFIG_SND_VMASTER="y"
      CONFIG_SND_AC97_CODEC="m"
      CONFIG_SND_INTEL8X0M="m"
    fi
  fi
  if alsa_check_kconfig_card "korg1212"; then
    if  ! ( test "$CONFIG_M68K" = "y" -o "$CONFIG_M68K" = "m" ) &&
      ( test "$CONFIG_SND_PCI" = "y" -o "$CONFIG_SND_PCI" = "m" ); then
      CONFIG_SND_TIMER="m"
      CONFIG_SND_PCM="m"
      CONFIG_SND_KORG1212="m"
    fi
  fi
  if alsa_check_kconfig_card "maestro3"; then
    if  ! ( test "$CONFIG_M68K" = "y" -o "$CONFIG_M68K" = "m" ) &&
      ( test "$CONFIG_SND_PCI" = "y" -o "$CONFIG_SND_PCI" = "m" ); then
      CONFIG_SND_TIMER="m"
      CONFIG_SND_PCM="m"
      test "$kversion.$kpatchlevel" = "2.6" -a $ksublevel -ge 14 && CONFIG_AC97_BUS="m"
      CONFIG_SND_VMASTER="y"
      CONFIG_SND_AC97_CODEC="m"
      CONFIG_SND_MAESTRO3="m"
    fi
  fi
  if alsa_check_kconfig_card "mixart"; then
    if  ! ( test "$CONFIG_M68K" = "y" -o "$CONFIG_M68K" = "m" ) &&
      ( test "$CONFIG_SND_PCI" = "y" -o "$CONFIG_SND_PCI" = "m" ); then
      CONFIG_SND_TIMER="m"
      CONFIG_SND_HWDEP="m"
      CONFIG_SND_PCM="m"
      CONFIG_SND_MIXART="m"
    fi
  fi
  if alsa_check_kconfig_card "nm256"; then
    if  ! ( test "$CONFIG_M68K" = "y" -o "$CONFIG_M68K" = "m" ) &&
      ( test "$CONFIG_SND_PCI" = "y" -o "$CONFIG_SND_PCI" = "m" ); then
      CONFIG_SND_TIMER="m"
      CONFIG_SND_PCM="m"
      test "$kversion.$kpatchlevel" = "2.6" -a $ksublevel -ge 14 && CONFIG_AC97_BUS="m"
      CONFIG_SND_VMASTER="y"
      CONFIG_SND_AC97_CODEC="m"
      CONFIG_SND_NM256="m"
    fi
  fi
  if alsa_check_kconfig_card "pcxhr"; then
    if  ! ( test "$CONFIG_M68K" = "y" -o "$CONFIG_M68K" = "m" ) &&
      ( test "$CONFIG_SND_PCI" = "y" -o "$CONFIG_SND_PCI" = "m" ); then
      CONFIG_SND_TIMER="m"
      CONFIG_SND_PCM="m"
      CONFIG_SND_HWDEP="m"
      CONFIG_SND_PCXHR="m"
    fi
  fi
  if alsa_check_kconfig_card "riptide"; then
    if  ! ( test "$CONFIG_M68K" = "y" -o "$CONFIG_M68K" = "m" ) &&
      ( test "$CONFIG_SND_PCI" = "y" -o "$CONFIG_SND_PCI" = "m" ) &&
      ( test "$CONFIG_FW_LOADER" = "y" -o "$CONFIG_FW_LOADER" = "m" ); then
      CONFIG_SND_TIMER="m"
      CONFIG_SND_HWDEP="m"
      CONFIG_SND_RAWMIDI="m"
      CONFIG_SND_PCM="m"
      test "$kversion.$kpatchlevel" = "2.6" -a $ksublevel -ge 14 && CONFIG_AC97_BUS="m"
      CONFIG_SND_VMASTER="y"
      CONFIG_SND_OPL3_LIB="m"
      CONFIG_SND_MPU401_UART="m"
      CONFIG_SND_AC97_CODEC="m"
      CONFIG_SND_RIPTIDE="m"
    fi
  fi
  if alsa_check_kconfig_card "rme32"; then
    if  ! ( test "$CONFIG_M68K" = "y" -o "$CONFIG_M68K" = "m" ) &&
      ( test "$CONFIG_SND_PCI" = "y" -o "$CONFIG_SND_PCI" = "m" ); then
      CONFIG_SND_TIMER="m"
      CONFIG_SND_PCM="m"
      CONFIG_SND_RME32="m"
    fi
  fi
  if alsa_check_kconfig_card "rme96"; then
    if  ! ( test "$CONFIG_M68K" = "y" -o "$CONFIG_M68K" = "m" ) &&
      ( test "$CONFIG_SND_PCI" = "y" -o "$CONFIG_SND_PCI" = "m" ); then
      CONFIG_SND_TIMER="m"
      CONFIG_SND_PCM="m"
      CONFIG_SND_RME96="m"
    fi
  fi
  if alsa_check_kconfig_card "rme9652"; then
    if  ! ( test "$CONFIG_M68K" = "y" -o "$CONFIG_M68K" = "m" ) &&
      ( test "$CONFIG_SND_PCI" = "y" -o "$CONFIG_SND_PCI" = "m" ); then
      CONFIG_SND_TIMER="m"
      CONFIG_SND_PCM="m"
      CONFIG_SND_RME9652="m"
    fi
  fi
  if alsa_check_kconfig_card "sis7019"; then
    if  ! ( test "$CONFIG_M68K" = "y" -o "$CONFIG_M68K" = "m" ) &&
      ( test "$CONFIG_SND_PCI" = "y" -o "$CONFIG_SND_PCI" = "m" ) &&
      ( ( test "$CONFIG_X86" = "y" -o "$CONFIG_X86" = "m" ) &&
       ! ( test "$CONFIG_X86_64" = "y" -o "$CONFIG_X86_64" = "m" ) ) &&
      ( test "$kversion.$kpatchlevel" = "2.6" ); then
      CONFIG_SND_TIMER="m"
      CONFIG_SND_PCM="m"
      test "$kversion.$kpatchlevel" = "2.6" -a $ksublevel -ge 14 && CONFIG_AC97_BUS="m"
      CONFIG_SND_VMASTER="y"
      CONFIG_SND_AC97_CODEC="m"
      CONFIG_SND_SIS7019="m"
    fi
  fi
  if alsa_check_kconfig_card "sonicvibes"; then
    if  ! ( test "$CONFIG_M68K" = "y" -o "$CONFIG_M68K" = "m" ) &&
      ( test "$CONFIG_SND_PCI" = "y" -o "$CONFIG_SND_PCI" = "m" ); then
      CONFIG_SND_TIMER="m"
      CONFIG_SND_HWDEP="m"
      CONFIG_SND_RAWMIDI="m"
      CONFIG_SND_PCM="m"
      test "$kversion.$kpatchlevel" = "2.6" -a $ksublevel -ge 14 && CONFIG_AC97_BUS="m"
      CONFIG_SND_VMASTER="y"
      CONFIG_SND_OPL3_LIB="m"
      CONFIG_SND_MPU401_UART="m"
      CONFIG_SND_AC97_CODEC="m"
      CONFIG_SND_SONICVIBES="m"
    fi
  fi
  if alsa_check_kconfig_card "trident"; then
    if  ! ( test "$CONFIG_M68K" = "y" -o "$CONFIG_M68K" = "m" ) &&
      ( test "$CONFIG_SND_PCI" = "y" -o "$CONFIG_SND_PCI" = "m" ); then
      CONFIG_SND_RAWMIDI="m"
      CONFIG_SND_TIMER="m"
      CONFIG_SND_PCM="m"
      test "$kversion.$kpatchlevel" = "2.6" -a $ksublevel -ge 14 && CONFIG_AC97_BUS="m"
      CONFIG_SND_VMASTER="y"
      CONFIG_SND_MPU401_UART="m"
      CONFIG_SND_AC97_CODEC="m"
      CONFIG_SND_TRIDENT="m"
    fi
  fi
  if alsa_check_kconfig_card "via82xx"; then
    if  ! ( test "$CONFIG_M68K" = "y" -o "$CONFIG_M68K" = "m" ) &&
      ( test "$CONFIG_SND_PCI" = "y" -o "$CONFIG_SND_PCI" = "m" ); then
      CONFIG_SND_RAWMIDI="m"
      CONFIG_SND_TIMER="m"
      CONFIG_SND_PCM="m"
      test "$kversion.$kpatchlevel" = "2.6" -a $ksublevel -ge 14 && CONFIG_AC97_BUS="m"
      CONFIG_SND_VMASTER="y"
      CONFIG_SND_MPU401_UART="m"
      CONFIG_SND_AC97_CODEC="m"
      CONFIG_SND_VIA82XX="m"
    fi
  fi
  if alsa_check_kconfig_card "via82xx-modem"; then
    if  ! ( test "$CONFIG_M68K" = "y" -o "$CONFIG_M68K" = "m" ) &&
      ( test "$CONFIG_SND_PCI" = "y" -o "$CONFIG_SND_PCI" = "m" ); then
      CONFIG_SND_TIMER="m"
      CONFIG_SND_PCM="m"
      test "$kversion.$kpatchlevel" = "2.6" -a $ksublevel -ge 14 && CONFIG_AC97_BUS="m"
      CONFIG_SND_VMASTER="y"
      CONFIG_SND_AC97_CODEC="m"
      CONFIG_SND_VIA82XX_MODEM="m"
    fi
  fi
  if alsa_check_kconfig_card "virtuoso"; then
    if  ! ( test "$CONFIG_M68K" = "y" -o "$CONFIG_M68K" = "m" ) &&
      ( test "$CONFIG_SND_PCI" = "y" -o "$CONFIG_SND_PCI" = "m" ); then
      CONFIG_SND_TIMER="m"
      CONFIG_SND_PCM="m"
      CONFIG_SND_RAWMIDI="m"
      CONFIG_SND_MPU401_UART="m"
      CONFIG_SND_OXYGEN_LIB="m"
      CONFIG_SND_VIRTUOSO="m"
    fi
  fi
  if alsa_check_kconfig_card "vx222"; then
    if  ! ( test "$CONFIG_M68K" = "y" -o "$CONFIG_M68K" = "m" ) &&
      ( test "$CONFIG_SND_PCI" = "y" -o "$CONFIG_SND_PCI" = "m" ); then
      CONFIG_SND_HWDEP="m"
      CONFIG_SND_TIMER="m"
      CONFIG_SND_PCM="m"
      CONFIG_SND_VX_LIB="m"
      CONFIG_SND_VX222="m"
    fi
  fi
  if alsa_check_kconfig_card "ymfpci"; then
    if  ! ( test "$CONFIG_M68K" = "y" -o "$CONFIG_M68K" = "m" ) &&
      ( test "$CONFIG_SND_PCI" = "y" -o "$CONFIG_SND_PCI" = "m" ); then
      CONFIG_SND_TIMER="m"
      CONFIG_SND_HWDEP="m"
      CONFIG_SND_RAWMIDI="m"
      CONFIG_SND_PCM="m"
      test "$kversion.$kpatchlevel" = "2.6" -a $ksublevel -ge 14 && CONFIG_AC97_BUS="m"
      CONFIG_SND_VMASTER="y"
      CONFIG_SND_OPL3_LIB="m"
      CONFIG_SND_MPU401_UART="m"
      CONFIG_SND_AC97_CODEC="m"
      CONFIG_SND_YMFPCI="m"
    fi
  fi
  if alsa_check_kconfig_card "pdplus"; then
    if  ! ( test "$CONFIG_M68K" = "y" -o "$CONFIG_M68K" = "m" ) &&
      ( test "$CONFIG_SND_PCI" = "y" -o "$CONFIG_SND_PCI" = "m" ); then
      CONFIG_SND_TIMER="m"
      CONFIG_SND_PCM="m"
      CONFIG_SND_PDPLUS="m"
    fi
  fi
  if alsa_check_kconfig_card "asihpi"; then
    if  ! ( test "$CONFIG_M68K" = "y" -o "$CONFIG_M68K" = "m" ) &&
      ( test "$CONFIG_SND_PCI" = "y" -o "$CONFIG_SND_PCI" = "m" ) &&
      ( test "$CONFIG_X86" = "y" -o "$CONFIG_X86" = "m" ) &&
      ( test "$CONFIG_FW_LOADER" = "y" -o "$CONFIG_FW_LOADER" = "m" ) &&
      ( test "$kversion.$kpatchlevel" = "2.6" -a $ksublevel -ge 10 ); then
      CONFIG_SND_TIMER="m"
      CONFIG_SND_PCM="m"
      CONFIG_SND_HWDEP="m"
      CONFIG_SND_ASIHPI="m"
    fi
  fi
    if  ! ( test "$CONFIG_M68K" = "y" -o "$CONFIG_M68K" = "m" ) &&
      ( ( test "$CONFIG_PPC64" = "y" -o "$CONFIG_PPC64" = "m" ) ||
      ( test "$CONFIG_PPC32" = "y" -o "$CONFIG_PPC32" = "m" ) ); then
      CONFIG_SND_PPC="y"
    fi
  if alsa_check_kconfig_card "powermac"; then
    if  ! ( test "$CONFIG_M68K" = "y" -o "$CONFIG_M68K" = "m" ) &&
      ( test "$CONFIG_SND_PPC" = "y" -o "$CONFIG_SND_PPC" = "m" ) &&
      ( ( test "$CONFIG_I2C" = "y" -o "$CONFIG_I2C" = "m" ) &&
      ( test "$CONFIG_INPUT" = "y" -o "$CONFIG_INPUT" = "m" ) &&
      ( test "$CONFIG_PPC_PMAC" = "y" -o "$CONFIG_PPC_PMAC" = "m" ) ); then
      CONFIG_SND_TIMER="m"
      CONFIG_SND_PCM="m"
      CONFIG_SND_POWERMAC="m"
    fi
  fi
  if alsa_check_kconfig_option "powermac-auto-drc"; then
    if  ! ( test "$CONFIG_M68K" = "y" -o "$CONFIG_M68K" = "m" ) &&
      ( test "$CONFIG_SND_PPC" = "y" -o "$CONFIG_SND_PPC" = "m" ) &&
      ( test "$CONFIG_SND_POWERMAC" = "y" -o "$CONFIG_SND_POWERMAC" = "m" ); then
      CONFIG_SND_POWERMAC_AUTO_DRC="y"
    fi
  fi
  if alsa_check_kconfig_card "ps3"; then
    if  ! ( test "$CONFIG_M68K" = "y" -o "$CONFIG_M68K" = "m" ) &&
      ( test "$CONFIG_SND_PPC" = "y" -o "$CONFIG_SND_PPC" = "m" ) &&
      ( test "$CONFIG_PS3_PS3AV" = "y" -o "$CONFIG_PS3_PS3AV" = "m" ); then
      CONFIG_SND_TIMER="m"
      CONFIG_SND_PCM="m"
      CONFIG_SND_PS3="m"
    fi
  fi
    if  ! ( test "$CONFIG_M68K" = "y" -o "$CONFIG_M68K" = "m" ) &&
      ( test "$CONFIG_SND_PPC" = "y" -o "$CONFIG_SND_PPC" = "m" ) &&
      ( test "$CONFIG_SND_PS3" = "y" -o "$CONFIG_SND_PS3" = "m" ); then
      CONFIG_SND_PS3_DEFAULT_START_DELAY="2000"
    fi
  if alsa_check_kconfig_card "aoa"; then
    if  ! ( test "$CONFIG_M68K" = "y" -o "$CONFIG_M68K" = "m" ) &&
      ( test "$CONFIG_PPC_PMAC" = "y" -o "$CONFIG_PPC_PMAC" = "m" ) &&
      ( test "$kversion.$kpatchlevel" = "2.6" -a $ksublevel -ge 16 ); then
      CONFIG_SND_TIMER="m"
      CONFIG_SND_PCM="m"
      CONFIG_SND_AOA="m"
    fi
  fi
  if alsa_check_kconfig_card "aoa-fabric-layout"; then
    if  ! ( test "$CONFIG_M68K" = "y" -o "$CONFIG_M68K" = "m" ) &&
      ( test "$CONFIG_SND_AOA" = "y" -o "$CONFIG_SND_AOA" = "m" ); then
      CONFIG_SND_TIMER="m"
      CONFIG_SND_PCM="m"
      CONFIG_SND_AOA_SOUNDBUS="m"
      CONFIG_SND_AOA_SOUNDBUS_I2S="m"
      CONFIG_SND_AOA_FABRIC_LAYOUT="m"
    fi
  fi
  if alsa_check_kconfig_card "aoa-onyx"; then
    if  ! ( test "$CONFIG_M68K" = "y" -o "$CONFIG_M68K" = "m" ) &&
      ( test "$CONFIG_SND_AOA" = "y" -o "$CONFIG_SND_AOA" = "m" ) &&
      ( test "$CONFIG_I2C" = "y" -o "$CONFIG_I2C" = "m" ) &&
      ( test "$CONFIG_I2C_POWERMAC" = "y" -o "$CONFIG_I2C_POWERMAC" = "m" ); then
      CONFIG_SND_AOA_ONYX="m"
    fi
  fi
  if alsa_check_kconfig_card "aoa-tas"; then
    if  ! ( test "$CONFIG_M68K" = "y" -o "$CONFIG_M68K" = "m" ) &&
      ( test "$CONFIG_SND_AOA" = "y" -o "$CONFIG_SND_AOA" = "m" ) &&
      ( test "$CONFIG_I2C" = "y" -o "$CONFIG_I2C" = "m" ) &&
      ( test "$CONFIG_I2C_POWERMAC" = "y" -o "$CONFIG_I2C_POWERMAC" = "m" ); then
      CONFIG_SND_AOA_TAS="m"
    fi
  fi
  if alsa_check_kconfig_card "aoa-toonie"; then
    if  ! ( test "$CONFIG_M68K" = "y" -o "$CONFIG_M68K" = "m" ) &&
      ( test "$CONFIG_SND_AOA" = "y" -o "$CONFIG_SND_AOA" = "m" ); then
      CONFIG_SND_AOA_TOONIE="m"
    fi
  fi
  if alsa_check_kconfig_card "aoa-soundbus"; then
    if  ! ( test "$CONFIG_M68K" = "y" -o "$CONFIG_M68K" = "m" ) &&
      ( test "$CONFIG_SND_AOA" = "y" -o "$CONFIG_SND_AOA" = "m" ); then
      CONFIG_SND_TIMER="m"
      CONFIG_SND_PCM="m"
      CONFIG_SND_AOA_SOUNDBUS="m"
    fi
  fi
  if alsa_check_kconfig_card "aoa-soundbus-i2s"; then
    if  ! ( test "$CONFIG_M68K" = "y" -o "$CONFIG_M68K" = "m" ) &&
      ( test "$CONFIG_SND_AOA" = "y" -o "$CONFIG_SND_AOA" = "m" ) &&
      ( ( test "$CONFIG_SND_AOA_SOUNDBUS" = "y" -o "$CONFIG_SND_AOA_SOUNDBUS" = "m" ) &&
      ( test "$CONFIG_PCI" = "y" -o "$CONFIG_PCI" = "m" ) ); then
      CONFIG_SND_AOA_SOUNDBUS_I2S="m"
    fi
  fi
    if  ! ( test "$CONFIG_M68K" = "y" -o "$CONFIG_M68K" = "m" ) &&
      ( test "$CONFIG_ARM" = "y" -o "$CONFIG_ARM" = "m" ); then
      CONFIG_SND_ARM="y"
    fi
  if alsa_check_kconfig_card "sa11xx-uda1341"; then
    if  ! ( test "$CONFIG_M68K" = "y" -o "$CONFIG_M68K" = "m" ) &&
      ( test "$CONFIG_SND_ARM" = "y" -o "$CONFIG_SND_ARM" = "m" ) &&
      ( ( test "$CONFIG_ARCH_SA1100" = "y" -o "$CONFIG_ARCH_SA1100" = "m" ) &&
      ( test "$CONFIG_L3" = "y" -o "$CONFIG_L3" = "m" ) ); then
      CONFIG_SND_TIMER="m"
      CONFIG_SND_PCM="m"
      CONFIG_SND_SA11XX_UDA1341="m"
    fi
  fi
  if alsa_check_kconfig_card "armaaci"; then
    if  ! ( test "$CONFIG_M68K" = "y" -o "$CONFIG_M68K" = "m" ) &&
      ( test "$CONFIG_SND_ARM" = "y" -o "$CONFIG_SND_ARM" = "m" ) &&
      ( test "$CONFIG_ARM_AMBA" = "y" -o "$CONFIG_ARM_AMBA" = "m" ); then
      CONFIG_SND_TIMER="m"
      CONFIG_SND_PCM="m"
      test "$kversion.$kpatchlevel" = "2.6" -a $ksublevel -ge 14 && CONFIG_AC97_BUS="m"
      CONFIG_SND_VMASTER="y"
      CONFIG_SND_PCM="m"
      CONFIG_SND_AC97_CODEC="m"
      CONFIG_SND_ARMAACI="m"
    fi
  fi
  if alsa_check_kconfig_card "soc"; then
    if  ! ( test "$CONFIG_M68K" = "y" -o "$CONFIG_M68K" = "m" ) &&
      ( test "$kversion.$kpatchlevel" = "2.6" -a $ksublevel -ge 10 ); then
      CONFIG_SND_TIMER="m"
      CONFIG_SND_PCM="m"
      test "$kversion.$kpatchlevel" = "2.6" -a $ksublevel -ge 14 && CONFIG_AC97_BUS="m"
      CONFIG_SND_SOC="m"
    fi
  fi
  if alsa_check_kconfig_card "pxa2xx-ac97"; then
    if  ! ( test "$CONFIG_M68K" = "y" -o "$CONFIG_M68K" = "m" ) &&
      ( test "$CONFIG_SND_ARM" = "y" -o "$CONFIG_SND_ARM" = "m" ) &&
      ( test "$CONFIG_ARCH_PXA" = "y" -o "$CONFIG_ARCH_PXA" = "m" ) &&
       ! ( ( test "$CONFIG_M68K" = "y" -o "$CONFIG_M68K" = "m" ) &&
      ( test "$CONFIG_SND_SOC" = "y" -o "$CONFIG_SND_SOC" = "m" ) ); then
      CONFIG_SND_TIMER="m"
      CONFIG_SND_PCM="m"
      test "$kversion.$kpatchlevel" = "2.6" -a $ksublevel -ge 14 && CONFIG_AC97_BUS="m"
      CONFIG_SND_VMASTER="y"
      CONFIG_SND_AC97_CODEC="m"
      test "$kversion.$kpatchlevel" = "2.6" && CONFIG_SND_PXA2XX_PCM="m"
      CONFIG_SND_AC97_CODEC="m"
      CONFIG_SND_PXA2XX_LIB="m"
      CONFIG_SND_PXA2XX_LIB_AC97="y"
      CONFIG_SND_AC97_CODEC="m"
      CONFIG_SND_PXA2XX_AC97="m"
    fi
  fi
  if alsa_check_kconfig_card "s3c2410"; then
    if  ! ( test "$CONFIG_M68K" = "y" -o "$CONFIG_M68K" = "m" ) &&
      ( test "$CONFIG_SND_ARM" = "y" -o "$CONFIG_SND_ARM" = "m" ) &&
      ( test "$CONFIG_ARCH_S3C2410" = "y" -o "$CONFIG_ARCH_S3C2410" = "m" ) &&
      ( test "$CONFIG_I2C_SENSOR" = "y" -o "$CONFIG_I2C_SENSOR" = "m" ); then
      CONFIG_SND_TIMER="m"
      CONFIG_SND_PCM="m"
      CONFIG_SND_S3C2410="m"
    fi
  fi
    if  ! ( test "$CONFIG_M68K" = "y" -o "$CONFIG_M68K" = "m" ) &&
      ( test "$CONFIG_SPI" = "y" -o "$CONFIG_SPI" = "m" ); then
      CONFIG_SND_SPI="y"
    fi
  if alsa_check_kconfig_card "at73c213"; then
    if  ! ( test "$CONFIG_M68K" = "y" -o "$CONFIG_M68K" = "m" ) &&
      ( test "$CONFIG_SND_SPI" = "y" -o "$CONFIG_SND_SPI" = "m" ) &&
      ( test "$CONFIG_ATMEL_SSC" = "y" -o "$CONFIG_ATMEL_SSC" = "m" ); then
      CONFIG_SND_TIMER="m"
      CONFIG_SND_PCM="m"
      CONFIG_SND_AT73C213="m"
    fi
  fi
    if  ! ( test "$CONFIG_M68K" = "y" -o "$CONFIG_M68K" = "m" ) &&
      ( test "$CONFIG_SND_SPI" = "y" -o "$CONFIG_SND_SPI" = "m" ) &&
      ( test "$CONFIG_SND_AT73C213" = "y" -o "$CONFIG_SND_AT73C213" = "m" ); then
      CONFIG_SND_AT73C213_TARGET_BITRATE="48000"
    fi
    if  ! ( test "$CONFIG_M68K" = "y" -o "$CONFIG_M68K" = "m" ) &&
      ( test "$CONFIG_MIPS" = "y" -o "$CONFIG_MIPS" = "m" ); then
      CONFIG_SND_MIPS="y"
    fi
  if alsa_check_kconfig_card "sgi-o2"; then
    if  ! ( test "$CONFIG_M68K" = "y" -o "$CONFIG_M68K" = "m" ) &&
      ( test "$CONFIG_SND_MIPS" = "y" -o "$CONFIG_SND_MIPS" = "m" ) &&
      ( test "$CONFIG_SGI_IP32" = "y" -o "$CONFIG_SGI_IP32" = "m" ); then
      CONFIG_SND_SGI_O2="m"
    fi
  fi
  if alsa_check_kconfig_card "sgi-hal2"; then
    if  ! ( test "$CONFIG_M68K" = "y" -o "$CONFIG_M68K" = "m" ) &&
      ( test "$CONFIG_SND_MIPS" = "y" -o "$CONFIG_SND_MIPS" = "m" ) &&
      ( test "$CONFIG_SGI_HAS_HAL2" = "y" -o "$CONFIG_SGI_HAS_HAL2" = "m" ); then
      CONFIG_SND_SGI_HAL2="m"
    fi
  fi
  if alsa_check_kconfig_card "au1x00"; then
    if  ! ( test "$CONFIG_M68K" = "y" -o "$CONFIG_M68K" = "m" ) &&
      ( test "$CONFIG_SND_MIPS" = "y" -o "$CONFIG_SND_MIPS" = "m" ) &&
      ( ( test "$CONFIG_SOC_AU1000" = "y" -o "$CONFIG_SOC_AU1000" = "m" ) ||
      ( test "$CONFIG_SOC_AU1100" = "y" -o "$CONFIG_SOC_AU1100" = "m" ) ||
      ( test "$CONFIG_SOC_AU1500" = "y" -o "$CONFIG_SOC_AU1500" = "m" ) ); then
      CONFIG_SND_TIMER="m"
      CONFIG_SND_PCM="m"
      test "$kversion.$kpatchlevel" = "2.6" -a $ksublevel -ge 14 && CONFIG_AC97_BUS="m"
      CONFIG_SND_VMASTER="y"
      CONFIG_SND_PCM="m"
      CONFIG_SND_AC97_CODEC="m"
      CONFIG_SND_AU1X00="m"
    fi
  fi
    if  ! ( test "$CONFIG_M68K" = "y" -o "$CONFIG_M68K" = "m" ) &&
      ( test "$CONFIG_SUPERH" = "y" -o "$CONFIG_SUPERH" = "m" ); then
      CONFIG_SND_SUPERH="y"
    fi
  if alsa_check_kconfig_card "aica"; then
    if  ! ( test "$CONFIG_M68K" = "y" -o "$CONFIG_M68K" = "m" ) &&
      ( test "$CONFIG_SND_SUPERH" = "y" -o "$CONFIG_SND_SUPERH" = "m" ) &&
      ( test "$CONFIG_SH_DREAMCAST" = "y" -o "$CONFIG_SH_DREAMCAST" = "m" ) &&
      ( test "$kversion.$kpatchlevel" = "2.6" ); then
      CONFIG_SND_TIMER="m"
      CONFIG_SND_PCM="m"
      CONFIG_SND_AICA="m"
    fi
  fi
    if  ! ( test "$CONFIG_M68K" = "y" -o "$CONFIG_M68K" = "m" ) &&
      ( test "$CONFIG_USB" = "y" -o "$CONFIG_USB" = "m" ); then
      CONFIG_SND_USB="y"
    fi
  if alsa_check_kconfig_card "usb-audio"; then
    if  ! ( test "$CONFIG_M68K" = "y" -o "$CONFIG_M68K" = "m" ) &&
      ( ( test "$CONFIG_SND_USB" = "y" -o "$CONFIG_SND_USB" = "m" ) &&
      ( test "$CONFIG_USB" = "y" -o "$CONFIG_USB" = "m" ) ); then
      CONFIG_SND_TIMER="m"
      CONFIG_SND_HWDEP="m"
      CONFIG_SND_RAWMIDI="m"
      CONFIG_SND_PCM="m"
      CONFIG_SND_USB_AUDIO="m"
    fi
  fi
  if alsa_check_kconfig_card "usb-usx2y"; then
    if  ! ( test "$CONFIG_M68K" = "y" -o "$CONFIG_M68K" = "m" ) &&
      ( ( test "$CONFIG_SND_USB" = "y" -o "$CONFIG_SND_USB" = "m" ) &&
      ( test "$CONFIG_USB" = "y" -o "$CONFIG_USB" = "m" ) ) &&
      ( ( test "$CONFIG_X86" = "y" -o "$CONFIG_X86" = "m" ) ||
      ( test "$CONFIG_PPC" = "y" -o "$CONFIG_PPC" = "m" ) ||
      ( test "$CONFIG_ALPHA" = "y" -o "$CONFIG_ALPHA" = "m" ) ); then
      CONFIG_SND_TIMER="m"
      CONFIG_SND_HWDEP="m"
      CONFIG_SND_RAWMIDI="m"
      CONFIG_SND_PCM="m"
      CONFIG_SND_USB_USX2Y="m"
    fi
  fi
  if alsa_check_kconfig_card "usb-caiaq"; then
    if  ! ( test "$CONFIG_M68K" = "y" -o "$CONFIG_M68K" = "m" ) &&
      ( ( test "$CONFIG_SND_USB" = "y" -o "$CONFIG_SND_USB" = "m" ) &&
      ( test "$CONFIG_USB" = "y" -o "$CONFIG_USB" = "m" ) ) &&
      ( test "$kversion.$kpatchlevel" = "2.6" -a $ksublevel -ge 15 ); then
      CONFIG_SND_TIMER="m"
      CONFIG_SND_HWDEP="m"
      CONFIG_SND_RAWMIDI="m"
      CONFIG_SND_PCM="m"
      CONFIG_SND_USB_CAIAQ="m"
    fi
  fi
  if alsa_check_kconfig_option "usb-caiaq-input"; then
    if  ! ( test "$CONFIG_M68K" = "y" -o "$CONFIG_M68K" = "m" ) &&
      ( ( test "$CONFIG_SND_USB" = "y" -o "$CONFIG_SND_USB" = "m" ) &&
      ( test "$CONFIG_USB" = "y" -o "$CONFIG_USB" = "m" ) ) &&
      ( test "$CONFIG_SND_USB_CAIAQ" = "y" -o "$CONFIG_SND_USB_CAIAQ" = "m" ) &&
      ( test "$CONFIG_INPUT" = "y" -o "$CONFIG_INPUT" = "m" ); then
      CONFIG_SND_USB_CAIAQ_INPUT="y"
    fi
  fi
  if alsa_check_kconfig_card "usb-us122l"; then
    if  ! ( test "$CONFIG_M68K" = "y" -o "$CONFIG_M68K" = "m" ) &&
      ( ( test "$CONFIG_SND_USB" = "y" -o "$CONFIG_SND_USB" = "m" ) &&
      ( test "$CONFIG_USB" = "y" -o "$CONFIG_USB" = "m" ) ) &&
      ( ( test "$CONFIG_X86" = "y" -o "$CONFIG_X86" = "m" ) &&
      ( test "$CONFIG_EXPERIMENTAL" = "y" -o "$CONFIG_EXPERIMENTAL" = "m" ) ) &&
      ( test "$kversion.$kpatchlevel" = "2.6" -a $ksublevel -ge 26 ); then
      CONFIG_SND_HWDEP="m"
      CONFIG_SND_RAWMIDI="m"
      CONFIG_SND_USB_US122L="m"
    fi
  fi
    if  ! ( test "$CONFIG_M68K" = "y" -o "$CONFIG_M68K" = "m" ) &&
      ( test "$CONFIG_PCMCIA" = "y" -o "$CONFIG_PCMCIA" = "m" ); then
      CONFIG_SND_PCMCIA="y"
    fi
  if alsa_check_kconfig_card "vxpocket"; then
    if  ! ( test "$CONFIG_M68K" = "y" -o "$CONFIG_M68K" = "m" ) &&
      ( ( test "$CONFIG_SND_PCMCIA" = "y" -o "$CONFIG_SND_PCMCIA" = "m" ) &&
      ( test "$CONFIG_PCMCIA" = "y" -o "$CONFIG_PCMCIA" = "m" ) ); then
      CONFIG_SND_HWDEP="m"
      CONFIG_SND_TIMER="m"
      CONFIG_SND_PCM="m"
      CONFIG_SND_VX_LIB="m"
      CONFIG_SND_VXPOCKET="m"
    fi
  fi
  if alsa_check_kconfig_card "pdaudiocf"; then
    if  ! ( test "$CONFIG_M68K" = "y" -o "$CONFIG_M68K" = "m" ) &&
      ( ( test "$CONFIG_SND_PCMCIA" = "y" -o "$CONFIG_SND_PCMCIA" = "m" ) &&
      ( test "$CONFIG_PCMCIA" = "y" -o "$CONFIG_PCMCIA" = "m" ) ); then
      CONFIG_SND_TIMER="m"
      CONFIG_SND_PCM="m"
      CONFIG_SND_PDAUDIOCF="m"
    fi
  fi
    if  ! ( test "$CONFIG_M68K" = "y" -o "$CONFIG_M68K" = "m" ) &&
      ( test "$CONFIG_SPARC" = "y" -o "$CONFIG_SPARC" = "m" ); then
      CONFIG_SND_SPARC="y"
    fi
  if alsa_check_kconfig_card "sun-amd7930"; then
    if  ! ( test "$CONFIG_M68K" = "y" -o "$CONFIG_M68K" = "m" ) &&
      ( test "$CONFIG_SND_SPARC" = "y" -o "$CONFIG_SND_SPARC" = "m" ) &&
      ( test "$CONFIG_SBUS" = "y" -o "$CONFIG_SBUS" = "m" ); then
      CONFIG_SND_TIMER="m"
      CONFIG_SND_PCM="m"
      CONFIG_SND_SUN_AMD7930="m"
    fi
  fi
  if alsa_check_kconfig_card "sun-cs4231"; then
    if  ! ( test "$CONFIG_M68K" = "y" -o "$CONFIG_M68K" = "m" ) &&
      ( test "$CONFIG_SND_SPARC" = "y" -o "$CONFIG_SND_SPARC" = "m" ); then
      CONFIG_SND_TIMER="m"
      CONFIG_SND_PCM="m"
      CONFIG_SND_SUN_CS4231="m"
    fi
  fi
  if alsa_check_kconfig_card "sun-dbri"; then
    if  ! ( test "$CONFIG_M68K" = "y" -o "$CONFIG_M68K" = "m" ) &&
      ( test "$CONFIG_SND_SPARC" = "y" -o "$CONFIG_SND_SPARC" = "m" ) &&
      ( test "$CONFIG_SBUS" = "y" -o "$CONFIG_SBUS" = "m" ); then
      CONFIG_SND_TIMER="m"
      CONFIG_SND_PCM="m"
      CONFIG_SND_SUN_DBRI="m"
    fi
  fi
    if  ! ( test "$CONFIG_M68K" = "y" -o "$CONFIG_M68K" = "m" ) &&
      ( test "$CONFIG_GSC" = "y" -o "$CONFIG_GSC" = "m" ); then
      CONFIG_SND_GSC="y"
    fi
  if alsa_check_kconfig_card "harmony"; then
    if  ! ( test "$CONFIG_M68K" = "y" -o "$CONFIG_M68K" = "m" ) &&
      ( test "$CONFIG_SND_GSC" = "y" -o "$CONFIG_SND_GSC" = "m" ); then
      CONFIG_SND_TIMER="m"
      CONFIG_SND_PCM="m"
      CONFIG_SND_HARMONY="m"
    fi
  fi
  if alsa_check_kconfig_card "at32-soc"; then
    if  ! ( test "$CONFIG_M68K" = "y" -o "$CONFIG_M68K" = "m" ) &&
      ( test "$CONFIG_SND_SOC" = "y" -o "$CONFIG_SND_SOC" = "m" ) &&
      ( ( test "$CONFIG_AVR32" = "y" -o "$CONFIG_AVR32" = "m" ) &&
      ( test "$CONFIG_SND_SOC" = "y" -o "$CONFIG_SND_SOC" = "m" ) ); then
      CONFIG_SND_AT32_SOC="m"
    fi
  fi
  if alsa_check_kconfig_card "at32-soc-playpaq"; then
    if  ! ( test "$CONFIG_M68K" = "y" -o "$CONFIG_M68K" = "m" ) &&
      ( test "$CONFIG_SND_SOC" = "y" -o "$CONFIG_SND_SOC" = "m" ) &&
      ( ( test "$CONFIG_SND_AT32_SOC" = "y" -o "$CONFIG_SND_AT32_SOC" = "m" ) &&
      ( test "$CONFIG_BOARD_PLAYPAQ" = "y" -o "$CONFIG_BOARD_PLAYPAQ" = "m" ) ); then
      CONFIG_SND_AT32_SOC_SSC="m"
      CONFIG_SND_SOC_WM8510="m"
      CONFIG_SND_AT32_SOC_PLAYPAQ="m"
    fi
  fi
  if alsa_check_kconfig_option "at32-soc-playpaq-slave"; then
    if  ! ( test "$CONFIG_M68K" = "y" -o "$CONFIG_M68K" = "m" ) &&
      ( test "$CONFIG_SND_SOC" = "y" -o "$CONFIG_SND_SOC" = "m" ) &&
      ( test "$CONFIG_SND_AT32_SOC_PLAYPAQ" = "y" -o "$CONFIG_SND_AT32_SOC_PLAYPAQ" = "m" ); then
      CONFIG_SND_AT32_SOC_PLAYPAQ_SLAVE="y"
    fi
  fi
  if alsa_check_kconfig_card "at91-soc"; then
    if  ! ( test "$CONFIG_M68K" = "y" -o "$CONFIG_M68K" = "m" ) &&
      ( test "$CONFIG_SND_SOC" = "y" -o "$CONFIG_SND_SOC" = "m" ) &&
      ( test "$CONFIG_ARCH_AT91" = "y" -o "$CONFIG_ARCH_AT91" = "m" ); then
      CONFIG_SND_AT91_SOC="m"
    fi
  fi
  if alsa_check_kconfig_card "soc-au1xpsc"; then
    if  ! ( test "$CONFIG_M68K" = "y" -o "$CONFIG_M68K" = "m" ) &&
      ( test "$CONFIG_SND_SOC" = "y" -o "$CONFIG_SND_SOC" = "m" ) &&
      ( ( test "$CONFIG_SOC_AU1200" = "y" -o "$CONFIG_SOC_AU1200" = "m" ) ||
      ( test "$CONFIG_SOC_AU1550" = "y" -o "$CONFIG_SOC_AU1550" = "m" ) ); then
      CONFIG_SND_SOC_AU1XPSC="m"
    fi
  fi
  if alsa_check_kconfig_card "soc-sample-psc-ac97"; then
    if  ! ( test "$CONFIG_M68K" = "y" -o "$CONFIG_M68K" = "m" ) &&
      ( test "$CONFIG_SND_SOC" = "y" -o "$CONFIG_SND_SOC" = "m" ) &&
      ( test "$CONFIG_SND_SOC_AU1XPSC" = "y" -o "$CONFIG_SND_SOC_AU1XPSC" = "m" ); then
      test "$kversion.$kpatchlevel" = "2.6" -a $ksublevel -ge 14 && CONFIG_AC97_BUS="m"
      CONFIG_SND_TIMER="m"
      CONFIG_SND_PCM="m"
      CONFIG_SND_VMASTER="y"
      CONFIG_SND_AC97_CODEC="m"
      CONFIG_SND_SOC_AC97_BUS="y"
      CONFIG_SND_SOC_AU1XPSC_AC97="m"
      CONFIG_SND_SOC_AC97_CODEC="m"
      CONFIG_SND_SOC_SAMPLE_PSC_AC97="m"
    fi
  fi
  if alsa_check_kconfig_card "pxa2xx-soc"; then
    if  ! ( test "$CONFIG_M68K" = "y" -o "$CONFIG_M68K" = "m" ) &&
      ( test "$CONFIG_SND_SOC" = "y" -o "$CONFIG_SND_SOC" = "m" ) &&
      ( test "$CONFIG_ARCH_PXA" = "y" -o "$CONFIG_ARCH_PXA" = "m" ); then
      CONFIG_SND_TIMER="m"
      CONFIG_SND_PCM="m"
      test "$kversion.$kpatchlevel" = "2.6" -a $ksublevel -ge 14 && CONFIG_AC97_BUS="m"
      CONFIG_SND_VMASTER="y"
      CONFIG_SND_AC97_CODEC="m"
      CONFIG_SND_PXA2XX_LIB="m"
      CONFIG_SND_PXA2XX_SOC="m"
    fi
  fi
  if alsa_check_kconfig_card "pxa2xx-soc-corgi"; then
    if  ! ( test "$CONFIG_M68K" = "y" -o "$CONFIG_M68K" = "m" ) &&
      ( test "$CONFIG_SND_SOC" = "y" -o "$CONFIG_SND_SOC" = "m" ) &&
      ( ( test "$CONFIG_SND_PXA2XX_SOC" = "y" -o "$CONFIG_SND_PXA2XX_SOC" = "m" ) &&
      ( test "$CONFIG_PXA_SHARP_C7xx" = "y" -o "$CONFIG_PXA_SHARP_C7xx" = "m" ) ); then
      CONFIG_SND_PXA2XX_SOC_I2S="m"
      CONFIG_SND_SOC_WM8731="m"
      CONFIG_SND_PXA2XX_SOC_CORGI="m"
    fi
  fi
  if alsa_check_kconfig_card "pxa2xx-soc-spitz"; then
    if  ! ( test "$CONFIG_M68K" = "y" -o "$CONFIG_M68K" = "m" ) &&
      ( test "$CONFIG_SND_SOC" = "y" -o "$CONFIG_SND_SOC" = "m" ) &&
      ( ( test "$CONFIG_SND_PXA2XX_SOC" = "y" -o "$CONFIG_SND_PXA2XX_SOC" = "m" ) &&
      ( test "$CONFIG_PXA_SHARP_Cxx00" = "y" -o "$CONFIG_PXA_SHARP_Cxx00" = "m" ) ); then
      CONFIG_SND_PXA2XX_SOC_I2S="m"
      CONFIG_SND_SOC_WM8750="m"
      CONFIG_SND_PXA2XX_SOC_SPITZ="m"
    fi
  fi
  if alsa_check_kconfig_card "pxa2xx-soc-poodle"; then
    if  ! ( test "$CONFIG_M68K" = "y" -o "$CONFIG_M68K" = "m" ) &&
      ( test "$CONFIG_SND_SOC" = "y" -o "$CONFIG_SND_SOC" = "m" ) &&
      ( ( test "$CONFIG_SND_PXA2XX_SOC" = "y" -o "$CONFIG_SND_PXA2XX_SOC" = "m" ) &&
      ( test "$CONFIG_MACH_POODLE" = "y" -o "$CONFIG_MACH_POODLE" = "m" ) ); then
      CONFIG_SND_PXA2XX_SOC_I2S="m"
      CONFIG_SND_SOC_WM8731="m"
      CONFIG_SND_PXA2XX_SOC_POODLE="m"
    fi
  fi
  if alsa_check_kconfig_card "pxa2xx-soc-tosa"; then
    if  ! ( test "$CONFIG_M68K" = "y" -o "$CONFIG_M68K" = "m" ) &&
      ( test "$CONFIG_SND_SOC" = "y" -o "$CONFIG_SND_SOC" = "m" ) &&
      ( ( test "$CONFIG_SND_PXA2XX_SOC" = "y" -o "$CONFIG_SND_PXA2XX_SOC" = "m" ) &&
      ( test "$CONFIG_MACH_TOSA" = "y" -o "$CONFIG_MACH_TOSA" = "m" ) ) &&
      ( test "$CONFIG_MFD_TC6393XB" = "y" -o "$CONFIG_MFD_TC6393XB" = "m" ); then
      test "$kversion.$kpatchlevel" = "2.6" -a $ksublevel -ge 14 && CONFIG_AC97_BUS="m"
      CONFIG_SND_ARM="y"
      CONFIG_SND_PXA2XX_LIB_AC97="y"
      CONFIG_SND_SOC_AC97_BUS="y"
      CONFIG_SND_PXA2XX_SOC_AC97="m"
      CONFIG_SND_SOC_WM9712="m"
      CONFIG_SND_PXA2XX_SOC_TOSA="m"
    fi
  fi
  if alsa_check_kconfig_card "pxa2xx-soc-e800"; then
    if  ! ( test "$CONFIG_M68K" = "y" -o "$CONFIG_M68K" = "m" ) &&
      ( test "$CONFIG_SND_SOC" = "y" -o "$CONFIG_SND_SOC" = "m" ) &&
      ( ( test "$CONFIG_SND_PXA2XX_SOC" = "y" -o "$CONFIG_SND_PXA2XX_SOC" = "m" ) &&
      ( test "$CONFIG_MACH_E800" = "y" -o "$CONFIG_MACH_E800" = "m" ) ); then
      test "$kversion.$kpatchlevel" = "2.6" -a $ksublevel -ge 14 && CONFIG_AC97_BUS="m"
      CONFIG_SND_ARM="y"
      CONFIG_SND_PXA2XX_LIB_AC97="y"
      CONFIG_SND_SOC_AC97_BUS="y"
      CONFIG_SND_SOC_WM9712="m"
      CONFIG_SND_PXA2XX_SOC_AC97="m"
      CONFIG_SND_PXA2XX_SOC_E800="m"
    fi
  fi
  if alsa_check_kconfig_card "pxa2xx-soc-em-x270"; then
    if  ! ( test "$CONFIG_M68K" = "y" -o "$CONFIG_M68K" = "m" ) &&
      ( test "$CONFIG_SND_SOC" = "y" -o "$CONFIG_SND_SOC" = "m" ) &&
      ( ( test "$CONFIG_SND_PXA2XX_SOC" = "y" -o "$CONFIG_SND_PXA2XX_SOC" = "m" ) &&
      ( test "$CONFIG_MACH_EM_X270" = "y" -o "$CONFIG_MACH_EM_X270" = "m" ) ); then
      test "$kversion.$kpatchlevel" = "2.6" -a $ksublevel -ge 14 && CONFIG_AC97_BUS="m"
      CONFIG_SND_ARM="y"
      CONFIG_SND_PXA2XX_LIB_AC97="y"
      CONFIG_SND_SOC_AC97_BUS="y"
      CONFIG_SND_PXA2XX_SOC_AC97="m"
      CONFIG_SND_SOC_WM9712="m"
      CONFIG_SND_PXA2XX_SOC_EM_X270="m"
    fi
  fi
  if alsa_check_kconfig_card "s3c24xx-soc"; then
    if  ! ( test "$CONFIG_M68K" = "y" -o "$CONFIG_M68K" = "m" ) &&
      ( test "$CONFIG_SND_SOC" = "y" -o "$CONFIG_SND_SOC" = "m" ) &&
      ( test "$CONFIG_ARCH_S3C2410" = "y" -o "$CONFIG_ARCH_S3C2410" = "m" ); then
      CONFIG_SND_S3C24XX_SOC="m"
    fi
  fi
  if alsa_check_kconfig_card "s3c24xx-soc-neo1973-wm8753"; then
    if  ! ( test "$CONFIG_M68K" = "y" -o "$CONFIG_M68K" = "m" ) &&
      ( test "$CONFIG_SND_SOC" = "y" -o "$CONFIG_SND_SOC" = "m" ) &&
      ( ( test "$CONFIG_SND_S3C24XX_SOC" = "y" -o "$CONFIG_SND_S3C24XX_SOC" = "m" ) &&
      ( test "$CONFIG_MACH_NEO1973_GTA01" = "y" -o "$CONFIG_MACH_NEO1973_GTA01" = "m" ) ); then
      CONFIG_SND_S3C24XX_SOC_I2S="m"
      CONFIG_SND_SOC_WM8753="m"
      CONFIG_SND_S3C24XX_SOC_NEO1973_WM8753="m"
    fi
  fi
  if alsa_check_kconfig_card "s3c24xx-soc-smdk2443-wm9710"; then
    if  ! ( test "$CONFIG_M68K" = "y" -o "$CONFIG_M68K" = "m" ) &&
      ( test "$CONFIG_SND_SOC" = "y" -o "$CONFIG_SND_SOC" = "m" ) &&
      ( ( test "$CONFIG_SND_S3C24XX_SOC" = "y" -o "$CONFIG_SND_S3C24XX_SOC" = "m" ) &&
      ( test "$CONFIG_MACH_SMDK2443" = "y" -o "$CONFIG_MACH_SMDK2443" = "m" ) ); then
      test "$kversion.$kpatchlevel" = "2.6" -a $ksublevel -ge 14 && CONFIG_AC97_BUS="m"
      CONFIG_SND_SOC_AC97_BUS="y"
      CONFIG_SND_TIMER="m"
      CONFIG_SND_PCM="m"
      CONFIG_SND_VMASTER="y"
      CONFIG_SND_AC97_CODEC="m"
      CONFIG_SND_S3C2443_SOC_AC97="m"
      CONFIG_SND_SOC_AC97_CODEC="m"
      CONFIG_SND_S3C24XX_SOC_SMDK2443_WM9710="m"
    fi
  fi
  if alsa_check_kconfig_card "s3c24xx-soc-ln2440sbc-alc650"; then
    if  ! ( test "$CONFIG_M68K" = "y" -o "$CONFIG_M68K" = "m" ) &&
      ( test "$CONFIG_SND_SOC" = "y" -o "$CONFIG_SND_SOC" = "m" ) &&
      ( test "$CONFIG_SND_S3C24XX_SOC" = "y" -o "$CONFIG_SND_S3C24XX_SOC" = "m" ); then
      test "$kversion.$kpatchlevel" = "2.6" -a $ksublevel -ge 14 && CONFIG_AC97_BUS="m"
      CONFIG_SND_SOC_AC97_BUS="y"
      CONFIG_SND_TIMER="m"
      CONFIG_SND_PCM="m"
      CONFIG_SND_VMASTER="y"
      CONFIG_SND_AC97_CODEC="m"
      CONFIG_SND_S3C2443_SOC_AC97="m"
      CONFIG_SND_SOC_AC97_CODEC="m"
      CONFIG_SND_S3C24XX_SOC_LN2440SBC_ALC650="m"
    fi
  fi
  if alsa_check_kconfig_card "soc-pcm-sh7760"; then
    if  ! ( test "$CONFIG_M68K" = "y" -o "$CONFIG_M68K" = "m" ) &&
      ( test "$CONFIG_SND_SOC" = "y" -o "$CONFIG_SND_SOC" = "m" ) &&
      ( test "$CONFIG_SUPERH" = "y" -o "$CONFIG_SUPERH" = "m" ) &&
      ( ( test "$CONFIG_CPU_SUBTYPE_SH7760" = "y" -o "$CONFIG_CPU_SUBTYPE_SH7760" = "m" ) &&
      ( test "$CONFIG_SH_DMABRG" = "y" -o "$CONFIG_SH_DMABRG" = "m" ) ); then
      CONFIG_SND_SOC_PCM_SH7760="m"
    fi
  fi
  if alsa_check_kconfig_card "sh7760-ac97"; then
    if  ! ( test "$CONFIG_M68K" = "y" -o "$CONFIG_M68K" = "m" ) &&
      ( test "$CONFIG_SND_SOC" = "y" -o "$CONFIG_SND_SOC" = "m" ) &&
      ( test "$CONFIG_SUPERH" = "y" -o "$CONFIG_SUPERH" = "m" ) &&
      ( ( test "$CONFIG_CPU_SUBTYPE_SH7760" = "y" -o "$CONFIG_CPU_SUBTYPE_SH7760" = "m" ) &&
      ( test "$CONFIG_SND_SOC_PCM_SH7760" = "y" -o "$CONFIG_SND_SOC_PCM_SH7760" = "m" ) ); then
      test "$kversion.$kpatchlevel" = "2.6" -a $ksublevel -ge 14 && CONFIG_AC97_BUS="m"
      CONFIG_SND_SOC_AC97_BUS="y"
      CONFIG_SND_TIMER="m"
      CONFIG_SND_PCM="m"
      CONFIG_SND_VMASTER="y"
      CONFIG_SND_AC97_CODEC="m"
      CONFIG_SND_SOC_SH4_HAC="m"
      CONFIG_SND_SOC_AC97_CODEC="m"
      CONFIG_SND_SH7760_AC97="m"
    fi
  fi
  if alsa_check_kconfig_option "soc-mpc8610"; then
    if  ! ( test "$CONFIG_M68K" = "y" -o "$CONFIG_M68K" = "m" ) &&
      ( test "$CONFIG_SND_SOC" = "y" -o "$CONFIG_SND_SOC" = "m" ) &&
      ( test "$CONFIG_MPC8610_HPCD" = "y" -o "$CONFIG_MPC8610_HPCD" = "m" ); then
      CONFIG_SND_SOC_MPC8610="y"
    fi
  fi
  if alsa_check_kconfig_option "soc-mpc8610-hpcd"; then
    if  ! ( test "$CONFIG_M68K" = "y" -o "$CONFIG_M68K" = "m" ) &&
      ( test "$CONFIG_SND_SOC" = "y" -o "$CONFIG_SND_SOC" = "m" ) &&
      ( test "$CONFIG_SND_SOC_MPC8610" = "y" -o "$CONFIG_SND_SOC_MPC8610" = "m" ); then
      CONFIG_SND_SOC_CS4270="m"
      CONFIG_SND_SOC_CS4270_VD33_ERRATA="y"
      CONFIG_SND_SOC_MPC8610_HPCD="y"
    fi
  fi
  if alsa_check_kconfig_card "soc-mpc5200-i2s"; then
    if  ! ( test "$CONFIG_M68K" = "y" -o "$CONFIG_M68K" = "m" ) &&
      ( test "$CONFIG_SND_SOC" = "y" -o "$CONFIG_SND_SOC" = "m" ) &&
      ( ( test "$CONFIG_SND_SOC" = "y" -o "$CONFIG_SND_SOC" = "m" ) &&
      ( test "$CONFIG_PPC_MPC52xx" = "y" -o "$CONFIG_PPC_MPC52xx" = "m" ) ); then
      CONFIG_SND_SOC_OF_SIMPLE="m"
      CONFIG_SND_SOC_MPC5200_I2S="m"
    fi
  fi
  if alsa_check_kconfig_card "davinci-soc"; then
    if  ! ( test "$CONFIG_M68K" = "y" -o "$CONFIG_M68K" = "m" ) &&
      ( test "$CONFIG_SND_SOC" = "y" -o "$CONFIG_SND_SOC" = "m" ) &&
      ( test "$CONFIG_ARCH_DAVINCI" = "y" -o "$CONFIG_ARCH_DAVINCI" = "m" ); then
      CONFIG_SND_DAVINCI_SOC="m"
    fi
  fi
  if alsa_check_kconfig_card "davinci-soc-evm"; then
    if  ! ( test "$CONFIG_M68K" = "y" -o "$CONFIG_M68K" = "m" ) &&
      ( test "$CONFIG_SND_SOC" = "y" -o "$CONFIG_SND_SOC" = "m" ) &&
      ( ( test "$CONFIG_SND_DAVINCI_SOC" = "y" -o "$CONFIG_SND_DAVINCI_SOC" = "m" ) &&
      ( test "$CONFIG_MACH_DAVINCI_EVM" = "y" -o "$CONFIG_MACH_DAVINCI_EVM" = "m" ) ); then
      CONFIG_SND_DAVINCI_SOC_I2S="m"
      CONFIG_SND_SOC_TLV320AIC3X="m"
      CONFIG_SND_DAVINCI_SOC_EVM="m"
    fi
  fi
  if alsa_check_kconfig_card "omap-soc"; then
    if  ! ( test "$CONFIG_M68K" = "y" -o "$CONFIG_M68K" = "m" ) &&
      ( test "$CONFIG_SND_SOC" = "y" -o "$CONFIG_SND_SOC" = "m" ) &&
      ( ( test "$CONFIG_ARCH_OMAP" = "y" -o "$CONFIG_ARCH_OMAP" = "m" ) &&
      ( test "$CONFIG_SND_SOC" = "y" -o "$CONFIG_SND_SOC" = "m" ) ); then
      CONFIG_SND_OMAP_SOC="m"
    fi
  fi
  if alsa_check_kconfig_card "omap-soc-n810"; then
    if  ! ( test "$CONFIG_M68K" = "y" -o "$CONFIG_M68K" = "m" ) &&
      ( test "$CONFIG_SND_SOC" = "y" -o "$CONFIG_SND_SOC" = "m" ) &&
      ( ( test "$CONFIG_SND_OMAP_SOC" = "y" -o "$CONFIG_SND_OMAP_SOC" = "m" ) &&
      ( test "$CONFIG_MACH_NOKIA_N810" = "y" -o "$CONFIG_MACH_NOKIA_N810" = "m" ) ); then
      CONFIG_SND_OMAP_SOC_MCBSP="m"
      CONFIG_SND_SOC_TLV320AIC3X="m"
      CONFIG_SND_OMAP_SOC_N810="m"
    fi
  fi
  if alsa_check_kconfig_card "omap-soc-osk5912"; then
    if  ! ( test "$CONFIG_M68K" = "y" -o "$CONFIG_M68K" = "m" ) &&
      ( test "$CONFIG_SND_SOC" = "y" -o "$CONFIG_SND_SOC" = "m" ) &&
      ( ( test "$CONFIG_SND_OMAP_SOC" = "y" -o "$CONFIG_SND_OMAP_SOC" = "m" ) &&
      ( test "$CONFIG_MACH_OMAP_OSK" = "y" -o "$CONFIG_MACH_OMAP_OSK" = "m" ) ); then
      CONFIG_SND_OMAP_SOC_MCBSP="m"
      CONFIG_SND_SOC_TLV320AIC23="m"
      CONFIG_SND_OMAP_SOC_OSK5912="m"
    fi
  fi
  if alsa_check_kconfig_card "bf5xx-i2s"; then
    if  ! ( test "$CONFIG_M68K" = "y" -o "$CONFIG_M68K" = "m" ) &&
      ( test "$CONFIG_SND_SOC" = "y" -o "$CONFIG_SND_SOC" = "m" ) &&
      ( ( test "$CONFIG_BLACKFIN" = "y" -o "$CONFIG_BLACKFIN" = "m" ) &&
      ( test "$CONFIG_SND_SOC" = "y" -o "$CONFIG_SND_SOC" = "m" ) ); then
      CONFIG_SND_BF5XX_I2S="m"
    fi
  fi
  if alsa_check_kconfig_card "bf5xx-soc-ssm2602"; then
    if  ! ( test "$CONFIG_M68K" = "y" -o "$CONFIG_M68K" = "m" ) &&
      ( test "$CONFIG_SND_SOC" = "y" -o "$CONFIG_SND_SOC" = "m" ) &&
      ( test "$CONFIG_SND_BF5XX_I2S" = "y" -o "$CONFIG_SND_BF5XX_I2S" = "m" ) &&
      ( test "$CONFIG_I2C" = "y" -o "$CONFIG_I2C" = "m" ) &&
      ( test "$CONFIG_I2C_BLACKFIN_TWI" = "y" -o "$CONFIG_I2C_BLACKFIN_TWI" = "m" ); then
      CONFIG_SND_BF5XX_SOC_SPORT="m"
      CONFIG_SND_BF5XX_SOC_I2S="m"
      CONFIG_SND_SOC_SSM2602="m"
      CONFIG_SND_BF5XX_SOC_SSM2602="m"
    fi
  fi
  if alsa_check_kconfig_card "bf5xx-soc-ad73311"; then
    if  ! ( test "$CONFIG_M68K" = "y" -o "$CONFIG_M68K" = "m" ) &&
      ( test "$CONFIG_SND_SOC" = "y" -o "$CONFIG_SND_SOC" = "m" ) &&
      ( test "$CONFIG_SND_BF5XX_I2S" = "y" -o "$CONFIG_SND_BF5XX_I2S" = "m" ); then
      CONFIG_SND_BF5XX_SOC_SPORT="m"
      CONFIG_SND_BF5XX_SOC_I2S="m"
      CONFIG_SND_SOC_AD73311="m"
      CONFIG_SND_BF5XX_SOC_AD73311="m"
    fi
  fi
    if  ! ( test "$CONFIG_M68K" = "y" -o "$CONFIG_M68K" = "m" ) &&
      ( test "$CONFIG_SND_SOC" = "y" -o "$CONFIG_SND_SOC" = "m" ) &&
      ( test "$CONFIG_SND_BF5XX_SOC_AD73311" = "y" -o "$CONFIG_SND_BF5XX_SOC_AD73311" = "m" ); then
      CONFIG_SND_BFIN_AD73311_SE="4"
    fi
  if alsa_check_kconfig_card "bf5xx-ac97"; then
    if  ! ( test "$CONFIG_M68K" = "y" -o "$CONFIG_M68K" = "m" ) &&
      ( test "$CONFIG_SND_SOC" = "y" -o "$CONFIG_SND_SOC" = "m" ) &&
      ( ( test "$CONFIG_BLACKFIN" = "y" -o "$CONFIG_BLACKFIN" = "m" ) &&
      ( test "$CONFIG_SND_SOC" = "y" -o "$CONFIG_SND_SOC" = "m" ) ); then
      CONFIG_SND_BF5XX_AC97="m"
    fi
  fi
  if alsa_check_kconfig_option "mmap-support"; then
    if  ! ( test "$CONFIG_M68K" = "y" -o "$CONFIG_M68K" = "m" ) &&
      ( test "$CONFIG_SND_SOC" = "y" -o "$CONFIG_SND_SOC" = "m" ) &&
      ( test "$CONFIG_SND_BF5XX_AC97" = "y" -o "$CONFIG_SND_BF5XX_AC97" = "m" ); then
      CONFIG_SND_MMAP_SUPPORT="y"
    fi
  fi
  if alsa_check_kconfig_card "bf5xx-soc-ad1980"; then
    if  ! ( test "$CONFIG_M68K" = "y" -o "$CONFIG_M68K" = "m" ) &&
      ( test "$CONFIG_SND_SOC" = "y" -o "$CONFIG_SND_SOC" = "m" ) &&
      ( test "$CONFIG_SND_BF5XX_AC97" = "y" -o "$CONFIG_SND_BF5XX_AC97" = "m" ); then
      test "$kversion.$kpatchlevel" = "2.6" -a $ksublevel -ge 14 && CONFIG_AC97_BUS="m"
      CONFIG_SND_SOC_AC97_BUS="y"
      CONFIG_SND_BF5XX_SOC_SPORT="m"
      CONFIG_SND_BF5XX_SOC_AC97="m"
      CONFIG_SND_SOC_AD1980="m"
      CONFIG_SND_BF5XX_SOC_AD1980="m"
    fi
  fi
    if  ! ( test "$CONFIG_M68K" = "y" -o "$CONFIG_M68K" = "m" ) &&
      ( test "$CONFIG_SND_SOC" = "y" -o "$CONFIG_SND_SOC" = "m" ) &&
      ( ( ( test "$CONFIG_SND_BF5XX_I2S" = "y" -o "$CONFIG_SND_BF5XX_I2S" = "m" ) ||
      ( test "$CONFIG_SND_BF5XX_AC97" = "y" -o "$CONFIG_SND_BF5XX_AC97" = "m" ) ) ); then
      CONFIG_SND_BF5XX_SPORT_NUM="0"
    fi
  if alsa_check_kconfig_option "bf5xx-have-cold-reset"; then
    if  ! ( test "$CONFIG_M68K" = "y" -o "$CONFIG_M68K" = "m" ) &&
      ( test "$CONFIG_SND_SOC" = "y" -o "$CONFIG_SND_SOC" = "m" ) &&
      ( test "$CONFIG_SND_BF5XX_AC97" = "y" -o "$CONFIG_SND_BF5XX_AC97" = "m" ); then
      CONFIG_SND_BF5XX_HAVE_COLD_RESET="y"
    fi
  fi
    if  ! ( test "$CONFIG_M68K" = "y" -o "$CONFIG_M68K" = "m" ) &&
      ( test "$CONFIG_SND_SOC" = "y" -o "$CONFIG_SND_SOC" = "m" ) &&
      ( test "$CONFIG_SND_BF5XX_HAVE_COLD_RESET" = "y" -o "$CONFIG_SND_BF5XX_HAVE_COLD_RESET" = "m" ); then
      CONFIG_SND_BF5XX_RESET_GPIO_NUM="5"
    fi
  if alsa_check_kconfig_card "soc-all-codecs"; then
    if  ! ( test "$CONFIG_M68K" = "y" -o "$CONFIG_M68K" = "m" ) &&
      ( test "$CONFIG_SND_SOC" = "y" -o "$CONFIG_SND_SOC" = "m" ) &&
      ( test "$CONFIG_I2C" = "y" -o "$CONFIG_I2C" = "m" ) &&
      ( test "$CONFIG_SPI" = "y" -o "$CONFIG_SPI" = "m" ) &&
      ( test "$CONFIG_SPI_MASTER" = "y" -o "$CONFIG_SPI_MASTER" = "m" ); then
      CONFIG_SND_SOC_AD73311="m"
      CONFIG_SND_SOC_AK4535="m"
      CONFIG_SND_SOC_CS4270="m"
      CONFIG_SND_SOC_SSM2602="m"
      CONFIG_SND_SOC_TLV320AIC23="m"
      CONFIG_SND_SOC_TLV320AIC26="m"
      CONFIG_SND_SOC_TLV320AIC3X="m"
      CONFIG_SND_SOC_UDA1380="m"
      CONFIG_SND_SOC_WM8510="m"
      CONFIG_SND_SOC_WM8580="m"
      CONFIG_SND_SOC_WM8731="m"
      CONFIG_SND_SOC_WM8750="m"
      CONFIG_SND_SOC_WM8753="m"
      CONFIG_SND_SOC_WM8900="m"
      CONFIG_SND_SOC_WM8903="m"
      CONFIG_SND_SOC_WM8971="m"
      CONFIG_SND_SOC_WM8990="m"
      CONFIG_SND_SOC_ALL_CODECS="m"
    fi
  fi
  if alsa_check_kconfig_card "soc-tlv320aic26"; then
    if  ! ( test "$CONFIG_M68K" = "y" -o "$CONFIG_M68K" = "m" ) &&
      ( test "$CONFIG_SND_SOC" = "y" -o "$CONFIG_SND_SOC" = "m" ) &&
      ( test "$CONFIG_SPI" = "y" -o "$CONFIG_SPI" = "m" ); then
      CONFIG_SND_SOC_TLV320AIC26="m"
    fi
  fi
])

AC_DEFUN([ALSA_TOPLEVEL_DEFINES], [
if test -n "$CONFIG_SOUND"; then
  AC_DEFINE(CONFIG_SOUND_MODULE)
fi
if test -n "$CONFIG_SOUND_OSS_CORE"; then
  AC_DEFINE(CONFIG_SOUND_OSS_CORE)
fi
if test -n "$CONFIG_SND"; then
  AC_DEFINE(CONFIG_SND_MODULE)
fi
if test -n "$CONFIG_SND_TIMER"; then
  AC_DEFINE(CONFIG_SND_TIMER_MODULE)
fi
if test -n "$CONFIG_SND_PCM"; then
  AC_DEFINE(CONFIG_SND_PCM_MODULE)
fi
if test -n "$CONFIG_SND_HWDEP"; then
  AC_DEFINE(CONFIG_SND_HWDEP_MODULE)
fi
if test -n "$CONFIG_SND_RAWMIDI"; then
  AC_DEFINE(CONFIG_SND_RAWMIDI_MODULE)
fi
if test -n "$CONFIG_SND_JACK"; then
  AC_DEFINE(CONFIG_SND_JACK)
fi
if test -n "$CONFIG_SND_SEQUENCER"; then
  AC_DEFINE(CONFIG_SND_SEQUENCER_MODULE)
fi
if test -n "$CONFIG_SND_SEQ_DUMMY"; then
  AC_DEFINE(CONFIG_SND_SEQ_DUMMY_MODULE)
fi
if test -n "$CONFIG_SND_OSSEMUL"; then
  AC_DEFINE(CONFIG_SND_OSSEMUL)
fi
if test -n "$CONFIG_SND_MIXER_OSS"; then
  AC_DEFINE(CONFIG_SND_MIXER_OSS_MODULE)
fi
if test -n "$CONFIG_SND_PCM_OSS"; then
  AC_DEFINE(CONFIG_SND_PCM_OSS_MODULE)
fi
if test -n "$CONFIG_SND_PCM_OSS_PLUGINS"; then
  AC_DEFINE(CONFIG_SND_PCM_OSS_PLUGINS)
fi
if test -n "$CONFIG_SND_SEQUENCER_OSS"; then
  AC_DEFINE(CONFIG_SND_SEQUENCER_OSS)
fi
if test -n "$CONFIG_SND_RTCTIMER"; then
  AC_DEFINE(CONFIG_SND_RTCTIMER_MODULE)
fi
if test -n "$CONFIG_SND_SEQ_RTCTIMER_DEFAULT"; then
  AC_DEFINE(CONFIG_SND_SEQ_RTCTIMER_DEFAULT)
fi
if test -n "$CONFIG_SND_DYNAMIC_MINORS"; then
  AC_DEFINE(CONFIG_SND_DYNAMIC_MINORS)
fi
if test -n "$CONFIG_SND_SUPPORT_OLD_API"; then
  AC_DEFINE(CONFIG_SND_SUPPORT_OLD_API)
fi
if test -n "$CONFIG_SND_VERBOSE_PROCFS"; then
  AC_DEFINE(CONFIG_SND_VERBOSE_PROCFS)
fi
if test -n "$CONFIG_SND_VERBOSE_PRINTK"; then
  AC_DEFINE(CONFIG_SND_VERBOSE_PRINTK)
fi
if test -n "$CONFIG_SND_DEBUG"; then
  AC_DEFINE(CONFIG_SND_DEBUG)
fi
if test -n "$CONFIG_SND_DEBUG_VERBOSE"; then
  AC_DEFINE(CONFIG_SND_DEBUG_VERBOSE)
fi
if test -n "$CONFIG_SND_PCM_XRUN_DEBUG"; then
  AC_DEFINE(CONFIG_SND_PCM_XRUN_DEBUG)
fi
if test -n "$CONFIG_SND_VMASTER"; then
  AC_DEFINE(CONFIG_SND_VMASTER)
fi
if test -n "$CONFIG_SND_BIT32_EMUL"; then
  AC_DEFINE(CONFIG_SND_BIT32_EMUL_MODULE)
fi
if test -n "$CONFIG_SND_DEBUG_MEMORY"; then
  AC_DEFINE(CONFIG_SND_DEBUG_MEMORY)
fi
if test -n "$CONFIG_SND_HPET"; then
  AC_DEFINE(CONFIG_SND_HPET_MODULE)
fi
if test -n "$CONFIG_SND_MPU401_UART"; then
  AC_DEFINE(CONFIG_SND_MPU401_UART_MODULE)
fi
if test -n "$CONFIG_SND_OPL3_LIB"; then
  AC_DEFINE(CONFIG_SND_OPL3_LIB_MODULE)
fi
if test -n "$CONFIG_SND_OPL4_LIB"; then
  AC_DEFINE(CONFIG_SND_OPL4_LIB_MODULE)
fi
if test -n "$CONFIG_SND_VX_LIB"; then
  AC_DEFINE(CONFIG_SND_VX_LIB_MODULE)
fi
if test -n "$CONFIG_SND_AC97_CODEC"; then
  AC_DEFINE(CONFIG_SND_AC97_CODEC_MODULE)
fi
if test -n "$CONFIG_SND_DRIVERS"; then
  AC_DEFINE(CONFIG_SND_DRIVERS)
fi
if test -n "$CONFIG_SND_PCSP"; then
  AC_DEFINE(CONFIG_SND_PCSP_MODULE)
fi
if test -n "$CONFIG_SND_DUMMY"; then
  AC_DEFINE(CONFIG_SND_DUMMY_MODULE)
fi
if test -n "$CONFIG_SND_VIRMIDI"; then
  AC_DEFINE(CONFIG_SND_VIRMIDI_MODULE)
fi
if test -n "$CONFIG_SND_MTPAV"; then
  AC_DEFINE(CONFIG_SND_MTPAV_MODULE)
fi
if test -n "$CONFIG_SND_MTS64"; then
  AC_DEFINE(CONFIG_SND_MTS64_MODULE)
fi
if test -n "$CONFIG_SND_SERIAL_U16550"; then
  AC_DEFINE(CONFIG_SND_SERIAL_U16550_MODULE)
fi
if test -n "$CONFIG_SND_MPU401"; then
  AC_DEFINE(CONFIG_SND_MPU401_MODULE)
fi
if test -n "$CONFIG_SND_PORTMAN2X4"; then
  AC_DEFINE(CONFIG_SND_PORTMAN2X4_MODULE)
fi
if test -n "$CONFIG_SND_ML403_AC97CR"; then
  AC_DEFINE(CONFIG_SND_ML403_AC97CR_MODULE)
fi
if test -n "$CONFIG_SND_AC97_POWER_SAVE"; then
  AC_DEFINE(CONFIG_SND_AC97_POWER_SAVE)
fi
if test -n "$CONFIG_SND_AC97_POWER_SAVE_DEFAULT"; then
  AC_DEFINE_UNQUOTED([CONFIG_SND_AC97_POWER_SAVE_DEFAULT], [0])
fi
if test -n "$CONFIG_SND_SERIALMIDI"; then
  AC_DEFINE(CONFIG_SND_SERIALMIDI_MODULE)
fi
if test -n "$CONFIG_SND_LOOPBACK"; then
  AC_DEFINE(CONFIG_SND_LOOPBACK_MODULE)
fi
if test -n "$CONFIG_SND_WSS_LIB"; then
  AC_DEFINE(CONFIG_SND_WSS_LIB_MODULE)
fi
if test -n "$CONFIG_SND_SB_COMMON"; then
  AC_DEFINE(CONFIG_SND_SB_COMMON_MODULE)
fi
if test -n "$CONFIG_SND_SB8_DSP"; then
  AC_DEFINE(CONFIG_SND_SB8_DSP_MODULE)
fi
if test -n "$CONFIG_SND_SB16_DSP"; then
  AC_DEFINE(CONFIG_SND_SB16_DSP_MODULE)
fi
if test -n "$CONFIG_SND_ISA"; then
  AC_DEFINE(CONFIG_SND_ISA)
fi
if test -n "$CONFIG_SND_ADLIB"; then
  AC_DEFINE(CONFIG_SND_ADLIB_MODULE)
fi
if test -n "$CONFIG_SND_AD1816A"; then
  AC_DEFINE(CONFIG_SND_AD1816A_MODULE)
fi
if test -n "$CONFIG_SND_AD1848"; then
  AC_DEFINE(CONFIG_SND_AD1848_MODULE)
fi
if test -n "$CONFIG_SND_ALS100"; then
  AC_DEFINE(CONFIG_SND_ALS100_MODULE)
fi
if test -n "$CONFIG_SND_AZT2320"; then
  AC_DEFINE(CONFIG_SND_AZT2320_MODULE)
fi
if test -n "$CONFIG_SND_CMI8330"; then
  AC_DEFINE(CONFIG_SND_CMI8330_MODULE)
fi
if test -n "$CONFIG_SND_CS4231"; then
  AC_DEFINE(CONFIG_SND_CS4231_MODULE)
fi
if test -n "$CONFIG_SND_CS4232"; then
  AC_DEFINE(CONFIG_SND_CS4232_MODULE)
fi
if test -n "$CONFIG_SND_CS4236"; then
  AC_DEFINE(CONFIG_SND_CS4236_MODULE)
fi
if test -n "$CONFIG_SND_DT019X"; then
  AC_DEFINE(CONFIG_SND_DT019X_MODULE)
fi
if test -n "$CONFIG_SND_ES968"; then
  AC_DEFINE(CONFIG_SND_ES968_MODULE)
fi
if test -n "$CONFIG_SND_ES1688"; then
  AC_DEFINE(CONFIG_SND_ES1688_MODULE)
fi
if test -n "$CONFIG_SND_ES18XX"; then
  AC_DEFINE(CONFIG_SND_ES18XX_MODULE)
fi
if test -n "$CONFIG_SND_SC6000"; then
  AC_DEFINE(CONFIG_SND_SC6000_MODULE)
fi
if test -n "$CONFIG_SND_GUSCLASSIC"; then
  AC_DEFINE(CONFIG_SND_GUSCLASSIC_MODULE)
fi
if test -n "$CONFIG_SND_GUSEXTREME"; then
  AC_DEFINE(CONFIG_SND_GUSEXTREME_MODULE)
fi
if test -n "$CONFIG_SND_GUSMAX"; then
  AC_DEFINE(CONFIG_SND_GUSMAX_MODULE)
fi
if test -n "$CONFIG_SND_INTERWAVE"; then
  AC_DEFINE(CONFIG_SND_INTERWAVE_MODULE)
fi
if test -n "$CONFIG_SND_INTERWAVE_STB"; then
  AC_DEFINE(CONFIG_SND_INTERWAVE_STB_MODULE)
fi
if test -n "$CONFIG_SND_OPL3SA2"; then
  AC_DEFINE(CONFIG_SND_OPL3SA2_MODULE)
fi
if test -n "$CONFIG_SND_OPTI92X_AD1848"; then
  AC_DEFINE(CONFIG_SND_OPTI92X_AD1848_MODULE)
fi
if test -n "$CONFIG_SND_OPTI92X_CS4231"; then
  AC_DEFINE(CONFIG_SND_OPTI92X_CS4231_MODULE)
fi
if test -n "$CONFIG_SND_OPTI93X"; then
  AC_DEFINE(CONFIG_SND_OPTI93X_MODULE)
fi
if test -n "$CONFIG_SND_MIRO"; then
  AC_DEFINE(CONFIG_SND_MIRO_MODULE)
fi
if test -n "$CONFIG_SND_SB8"; then
  AC_DEFINE(CONFIG_SND_SB8_MODULE)
fi
if test -n "$CONFIG_SND_SB16"; then
  AC_DEFINE(CONFIG_SND_SB16_MODULE)
fi
if test -n "$CONFIG_SND_SBAWE"; then
  AC_DEFINE(CONFIG_SND_SBAWE_MODULE)
fi
if test -n "$CONFIG_SND_SB16_CSP"; then
  AC_DEFINE(CONFIG_SND_SB16_CSP)
fi
if test -n "$CONFIG_SND_SGALAXY"; then
  AC_DEFINE(CONFIG_SND_SGALAXY_MODULE)
fi
if test -n "$CONFIG_SND_SSCAPE"; then
  AC_DEFINE(CONFIG_SND_SSCAPE_MODULE)
fi
if test -n "$CONFIG_SND_WAVEFRONT"; then
  AC_DEFINE(CONFIG_SND_WAVEFRONT_MODULE)
fi
if test -n "$CONFIG_SND_WAVEFRONT_FIRMWARE_IN_KERNEL"; then
  AC_DEFINE(CONFIG_SND_WAVEFRONT_FIRMWARE_IN_KERNEL)
fi
if test -n "$CONFIG_SND_PC98_CS4232"; then
  AC_DEFINE(CONFIG_SND_PC98_CS4232_MODULE)
fi
if test -n "$CONFIG_SND_MSND_PINNACLE"; then
  AC_DEFINE(CONFIG_SND_MSND_PINNACLE_MODULE)
fi
if test -n "$CONFIG_SND_PCI"; then
  AC_DEFINE(CONFIG_SND_PCI)
fi
if test -n "$CONFIG_SND_AD1889"; then
  AC_DEFINE(CONFIG_SND_AD1889_MODULE)
fi
if test -n "$CONFIG_SND_ALS300"; then
  AC_DEFINE(CONFIG_SND_ALS300_MODULE)
fi
if test -n "$CONFIG_SND_ALS4000"; then
  AC_DEFINE(CONFIG_SND_ALS4000_MODULE)
fi
if test -n "$CONFIG_SND_ALI5451"; then
  AC_DEFINE(CONFIG_SND_ALI5451_MODULE)
fi
if test -n "$CONFIG_SND_ATIIXP"; then
  AC_DEFINE(CONFIG_SND_ATIIXP_MODULE)
fi
if test -n "$CONFIG_SND_ATIIXP_MODEM"; then
  AC_DEFINE(CONFIG_SND_ATIIXP_MODEM_MODULE)
fi
if test -n "$CONFIG_SND_AU8810"; then
  AC_DEFINE(CONFIG_SND_AU8810_MODULE)
fi
if test -n "$CONFIG_SND_AU8820"; then
  AC_DEFINE(CONFIG_SND_AU8820_MODULE)
fi
if test -n "$CONFIG_SND_AU8830"; then
  AC_DEFINE(CONFIG_SND_AU8830_MODULE)
fi
if test -n "$CONFIG_SND_AW2"; then
  AC_DEFINE(CONFIG_SND_AW2_MODULE)
fi
if test -n "$CONFIG_SND_AZT3328"; then
  AC_DEFINE(CONFIG_SND_AZT3328_MODULE)
fi
if test -n "$CONFIG_SND_BT87X"; then
  AC_DEFINE(CONFIG_SND_BT87X_MODULE)
fi
if test -n "$CONFIG_SND_BT87X_OVERCLOCK"; then
  AC_DEFINE(CONFIG_SND_BT87X_OVERCLOCK)
fi
if test -n "$CONFIG_SND_CA0106"; then
  AC_DEFINE(CONFIG_SND_CA0106_MODULE)
fi
if test -n "$CONFIG_SND_CMIPCI"; then
  AC_DEFINE(CONFIG_SND_CMIPCI_MODULE)
fi
if test -n "$CONFIG_SND_OXYGEN_LIB"; then
  AC_DEFINE(CONFIG_SND_OXYGEN_LIB_MODULE)
fi
if test -n "$CONFIG_SND_OXYGEN"; then
  AC_DEFINE(CONFIG_SND_OXYGEN_MODULE)
fi
if test -n "$CONFIG_SND_CS4281"; then
  AC_DEFINE(CONFIG_SND_CS4281_MODULE)
fi
if test -n "$CONFIG_SND_CS46XX"; then
  AC_DEFINE(CONFIG_SND_CS46XX_MODULE)
fi
if test -n "$CONFIG_SND_CS46XX_NEW_DSP"; then
  AC_DEFINE(CONFIG_SND_CS46XX_NEW_DSP)
fi
if test -n "$CONFIG_SND_CS5530"; then
  AC_DEFINE(CONFIG_SND_CS5530_MODULE)
fi
if test -n "$CONFIG_SND_CS5535AUDIO"; then
  AC_DEFINE(CONFIG_SND_CS5535AUDIO_MODULE)
fi
if test -n "$CONFIG_SND_DARLA20"; then
  AC_DEFINE(CONFIG_SND_DARLA20_MODULE)
fi
if test -n "$CONFIG_SND_GINA20"; then
  AC_DEFINE(CONFIG_SND_GINA20_MODULE)
fi
if test -n "$CONFIG_SND_LAYLA20"; then
  AC_DEFINE(CONFIG_SND_LAYLA20_MODULE)
fi
if test -n "$CONFIG_SND_DARLA24"; then
  AC_DEFINE(CONFIG_SND_DARLA24_MODULE)
fi
if test -n "$CONFIG_SND_GINA24"; then
  AC_DEFINE(CONFIG_SND_GINA24_MODULE)
fi
if test -n "$CONFIG_SND_LAYLA24"; then
  AC_DEFINE(CONFIG_SND_LAYLA24_MODULE)
fi
if test -n "$CONFIG_SND_MONA"; then
  AC_DEFINE(CONFIG_SND_MONA_MODULE)
fi
if test -n "$CONFIG_SND_MIA"; then
  AC_DEFINE(CONFIG_SND_MIA_MODULE)
fi
if test -n "$CONFIG_SND_ECHO3G"; then
  AC_DEFINE(CONFIG_SND_ECHO3G_MODULE)
fi
if test -n "$CONFIG_SND_INDIGO"; then
  AC_DEFINE(CONFIG_SND_INDIGO_MODULE)
fi
if test -n "$CONFIG_SND_INDIGOIO"; then
  AC_DEFINE(CONFIG_SND_INDIGOIO_MODULE)
fi
if test -n "$CONFIG_SND_INDIGODJ"; then
  AC_DEFINE(CONFIG_SND_INDIGODJ_MODULE)
fi
if test -n "$CONFIG_SND_EMU10K1"; then
  AC_DEFINE(CONFIG_SND_EMU10K1_MODULE)
fi
if test -n "$CONFIG_SND_EMU10K1X"; then
  AC_DEFINE(CONFIG_SND_EMU10K1X_MODULE)
fi
if test -n "$CONFIG_SND_ENS1370"; then
  AC_DEFINE(CONFIG_SND_ENS1370_MODULE)
fi
if test -n "$CONFIG_SND_ENS1371"; then
  AC_DEFINE(CONFIG_SND_ENS1371_MODULE)
fi
if test -n "$CONFIG_SND_ES1938"; then
  AC_DEFINE(CONFIG_SND_ES1938_MODULE)
fi
if test -n "$CONFIG_SND_ES1968"; then
  AC_DEFINE(CONFIG_SND_ES1968_MODULE)
fi
if test -n "$CONFIG_SND_FM801"; then
  AC_DEFINE(CONFIG_SND_FM801_MODULE)
fi
if test -n "$CONFIG_SND_FM801_TEA575X_BOOL"; then
  AC_DEFINE(CONFIG_SND_FM801_TEA575X_BOOL)
fi
if test -n "$CONFIG_SND_FM801_TEA575X"; then
  AC_DEFINE(CONFIG_SND_FM801_TEA575X_MODULE)
fi
if test -n "$CONFIG_SND_HDA_INTEL"; then
  AC_DEFINE(CONFIG_SND_HDA_INTEL_MODULE)
fi
if test -n "$CONFIG_SND_HDA_HWDEP"; then
  AC_DEFINE(CONFIG_SND_HDA_HWDEP)
fi
if test -n "$CONFIG_SND_HDA_INPUT_BEEP"; then
  AC_DEFINE(CONFIG_SND_HDA_INPUT_BEEP)
fi
if test -n "$CONFIG_SND_HDA_CODEC_REALTEK"; then
  AC_DEFINE(CONFIG_SND_HDA_CODEC_REALTEK)
fi
if test -n "$CONFIG_SND_HDA_CODEC_ANALOG"; then
  AC_DEFINE(CONFIG_SND_HDA_CODEC_ANALOG)
fi
if test -n "$CONFIG_SND_HDA_CODEC_SIGMATEL"; then
  AC_DEFINE(CONFIG_SND_HDA_CODEC_SIGMATEL)
fi
if test -n "$CONFIG_SND_HDA_CODEC_VIA"; then
  AC_DEFINE(CONFIG_SND_HDA_CODEC_VIA)
fi
if test -n "$CONFIG_SND_HDA_CODEC_ATIHDMI"; then
  AC_DEFINE(CONFIG_SND_HDA_CODEC_ATIHDMI)
fi
if test -n "$CONFIG_SND_HDA_CODEC_NVHDMI"; then
  AC_DEFINE(CONFIG_SND_HDA_CODEC_NVHDMI)
fi
if test -n "$CONFIG_SND_HDA_CODEC_CONEXANT"; then
  AC_DEFINE(CONFIG_SND_HDA_CODEC_CONEXANT)
fi
if test -n "$CONFIG_SND_HDA_CODEC_CMEDIA"; then
  AC_DEFINE(CONFIG_SND_HDA_CODEC_CMEDIA)
fi
if test -n "$CONFIG_SND_HDA_CODEC_SI3054"; then
  AC_DEFINE(CONFIG_SND_HDA_CODEC_SI3054)
fi
if test -n "$CONFIG_SND_HDA_GENERIC"; then
  AC_DEFINE(CONFIG_SND_HDA_GENERIC)
fi
if test -n "$CONFIG_SND_HDA_POWER_SAVE"; then
  AC_DEFINE(CONFIG_SND_HDA_POWER_SAVE)
fi
if test -n "$CONFIG_SND_HDA_POWER_SAVE_DEFAULT"; then
  AC_DEFINE_UNQUOTED([CONFIG_SND_HDA_POWER_SAVE_DEFAULT], [0])
fi
if test -n "$CONFIG_SND_HDSP"; then
  AC_DEFINE(CONFIG_SND_HDSP_MODULE)
fi
if test -n "$CONFIG_SND_HDSPM"; then
  AC_DEFINE(CONFIG_SND_HDSPM_MODULE)
fi
if test -n "$CONFIG_SND_HIFIER"; then
  AC_DEFINE(CONFIG_SND_HIFIER_MODULE)
fi
if test -n "$CONFIG_SND_ICE1712"; then
  AC_DEFINE(CONFIG_SND_ICE1712_MODULE)
fi
if test -n "$CONFIG_SND_ICE1724"; then
  AC_DEFINE(CONFIG_SND_ICE1724_MODULE)
fi
if test -n "$CONFIG_SND_INTEL8X0"; then
  AC_DEFINE(CONFIG_SND_INTEL8X0_MODULE)
fi
if test -n "$CONFIG_SND_INTEL8X0M"; then
  AC_DEFINE(CONFIG_SND_INTEL8X0M_MODULE)
fi
if test -n "$CONFIG_SND_KORG1212"; then
  AC_DEFINE(CONFIG_SND_KORG1212_MODULE)
fi
if test -n "$CONFIG_SND_MAESTRO3"; then
  AC_DEFINE(CONFIG_SND_MAESTRO3_MODULE)
fi
if test -n "$CONFIG_SND_MIXART"; then
  AC_DEFINE(CONFIG_SND_MIXART_MODULE)
fi
if test -n "$CONFIG_SND_NM256"; then
  AC_DEFINE(CONFIG_SND_NM256_MODULE)
fi
if test -n "$CONFIG_SND_PCXHR"; then
  AC_DEFINE(CONFIG_SND_PCXHR_MODULE)
fi
if test -n "$CONFIG_SND_RIPTIDE"; then
  AC_DEFINE(CONFIG_SND_RIPTIDE_MODULE)
fi
if test -n "$CONFIG_SND_RME32"; then
  AC_DEFINE(CONFIG_SND_RME32_MODULE)
fi
if test -n "$CONFIG_SND_RME96"; then
  AC_DEFINE(CONFIG_SND_RME96_MODULE)
fi
if test -n "$CONFIG_SND_RME9652"; then
  AC_DEFINE(CONFIG_SND_RME9652_MODULE)
fi
if test -n "$CONFIG_SND_SIS7019"; then
  AC_DEFINE(CONFIG_SND_SIS7019_MODULE)
fi
if test -n "$CONFIG_SND_SONICVIBES"; then
  AC_DEFINE(CONFIG_SND_SONICVIBES_MODULE)
fi
if test -n "$CONFIG_SND_TRIDENT"; then
  AC_DEFINE(CONFIG_SND_TRIDENT_MODULE)
fi
if test -n "$CONFIG_SND_VIA82XX"; then
  AC_DEFINE(CONFIG_SND_VIA82XX_MODULE)
fi
if test -n "$CONFIG_SND_VIA82XX_MODEM"; then
  AC_DEFINE(CONFIG_SND_VIA82XX_MODEM_MODULE)
fi
if test -n "$CONFIG_SND_VIRTUOSO"; then
  AC_DEFINE(CONFIG_SND_VIRTUOSO_MODULE)
fi
if test -n "$CONFIG_SND_VX222"; then
  AC_DEFINE(CONFIG_SND_VX222_MODULE)
fi
if test -n "$CONFIG_SND_YMFPCI"; then
  AC_DEFINE(CONFIG_SND_YMFPCI_MODULE)
fi
if test -n "$CONFIG_SND_PDPLUS"; then
  AC_DEFINE(CONFIG_SND_PDPLUS_MODULE)
fi
if test -n "$CONFIG_SND_ASIHPI"; then
  AC_DEFINE(CONFIG_SND_ASIHPI_MODULE)
fi
if test -n "$CONFIG_SND_PPC"; then
  AC_DEFINE(CONFIG_SND_PPC)
fi
if test -n "$CONFIG_SND_POWERMAC"; then
  AC_DEFINE(CONFIG_SND_POWERMAC_MODULE)
fi
if test -n "$CONFIG_SND_POWERMAC_AUTO_DRC"; then
  AC_DEFINE(CONFIG_SND_POWERMAC_AUTO_DRC)
fi
if test -n "$CONFIG_SND_PS3"; then
  AC_DEFINE(CONFIG_SND_PS3_MODULE)
fi
if test -n "$CONFIG_SND_PS3_DEFAULT_START_DELAY"; then
  AC_DEFINE_UNQUOTED([CONFIG_SND_PS3_DEFAULT_START_DELAY], [2000])
fi
if test -n "$CONFIG_SND_AOA"; then
  AC_DEFINE(CONFIG_SND_AOA_MODULE)
fi
if test -n "$CONFIG_SND_AOA_FABRIC_LAYOUT"; then
  AC_DEFINE(CONFIG_SND_AOA_FABRIC_LAYOUT_MODULE)
fi
if test -n "$CONFIG_SND_AOA_ONYX"; then
  AC_DEFINE(CONFIG_SND_AOA_ONYX_MODULE)
fi
if test -n "$CONFIG_SND_AOA_TAS"; then
  AC_DEFINE(CONFIG_SND_AOA_TAS_MODULE)
fi
if test -n "$CONFIG_SND_AOA_TOONIE"; then
  AC_DEFINE(CONFIG_SND_AOA_TOONIE_MODULE)
fi
if test -n "$CONFIG_SND_AOA_SOUNDBUS"; then
  AC_DEFINE(CONFIG_SND_AOA_SOUNDBUS_MODULE)
fi
if test -n "$CONFIG_SND_AOA_SOUNDBUS_I2S"; then
  AC_DEFINE(CONFIG_SND_AOA_SOUNDBUS_I2S_MODULE)
fi
if test -n "$CONFIG_SND_ARM"; then
  AC_DEFINE(CONFIG_SND_ARM)
fi
if test -n "$CONFIG_SND_SA11XX_UDA1341"; then
  AC_DEFINE(CONFIG_SND_SA11XX_UDA1341_MODULE)
fi
if test -n "$CONFIG_SND_ARMAACI"; then
  AC_DEFINE(CONFIG_SND_ARMAACI_MODULE)
fi
if test -n "$CONFIG_SND_PXA2XX_PCM"; then
  AC_DEFINE(CONFIG_SND_PXA2XX_PCM_MODULE)
fi
if test -n "$CONFIG_SND_PXA2XX_LIB"; then
  AC_DEFINE(CONFIG_SND_PXA2XX_LIB_MODULE)
fi
if test -n "$CONFIG_SND_PXA2XX_LIB_AC97"; then
  AC_DEFINE(CONFIG_SND_PXA2XX_LIB_AC97)
fi
if test -n "$CONFIG_SND_PXA2XX_AC97"; then
  AC_DEFINE(CONFIG_SND_PXA2XX_AC97_MODULE)
fi
if test -n "$CONFIG_SND_S3C2410"; then
  AC_DEFINE(CONFIG_SND_S3C2410_MODULE)
fi
if test -n "$CONFIG_SND_PXA2XX_I2SOUND"; then
  AC_DEFINE(CONFIG_SND_PXA2XX_I2SOUND_MODULE)
fi
if test -n "$CONFIG_SND_SPI"; then
  AC_DEFINE(CONFIG_SND_SPI)
fi
if test -n "$CONFIG_SND_AT73C213"; then
  AC_DEFINE(CONFIG_SND_AT73C213_MODULE)
fi
if test -n "$CONFIG_SND_AT73C213_TARGET_BITRATE"; then
  AC_DEFINE_UNQUOTED([CONFIG_SND_AT73C213_TARGET_BITRATE], [48000])
fi
if test -n "$CONFIG_SND_MIPS"; then
  AC_DEFINE(CONFIG_SND_MIPS)
fi
if test -n "$CONFIG_SND_SGI_O2"; then
  AC_DEFINE(CONFIG_SND_SGI_O2_MODULE)
fi
if test -n "$CONFIG_SND_SGI_HAL2"; then
  AC_DEFINE(CONFIG_SND_SGI_HAL2_MODULE)
fi
if test -n "$CONFIG_SND_AU1X00"; then
  AC_DEFINE(CONFIG_SND_AU1X00_MODULE)
fi
if test -n "$CONFIG_SND_SUPERH"; then
  AC_DEFINE(CONFIG_SND_SUPERH)
fi
if test -n "$CONFIG_SND_AICA"; then
  AC_DEFINE(CONFIG_SND_AICA_MODULE)
fi
if test -n "$CONFIG_SND_USB"; then
  AC_DEFINE(CONFIG_SND_USB)
fi
if test -n "$CONFIG_SND_USB_AUDIO"; then
  AC_DEFINE(CONFIG_SND_USB_AUDIO_MODULE)
fi
if test -n "$CONFIG_SND_USB_USX2Y"; then
  AC_DEFINE(CONFIG_SND_USB_USX2Y_MODULE)
fi
if test -n "$CONFIG_SND_USB_CAIAQ"; then
  AC_DEFINE(CONFIG_SND_USB_CAIAQ_MODULE)
fi
if test -n "$CONFIG_SND_USB_CAIAQ_INPUT"; then
  AC_DEFINE(CONFIG_SND_USB_CAIAQ_INPUT)
fi
if test -n "$CONFIG_SND_USB_US122L"; then
  AC_DEFINE(CONFIG_SND_USB_US122L_MODULE)
fi
if test -n "$CONFIG_SND_PCMCIA"; then
  AC_DEFINE(CONFIG_SND_PCMCIA)
fi
if test -n "$CONFIG_SND_VXPOCKET"; then
  AC_DEFINE(CONFIG_SND_VXPOCKET_MODULE)
fi
if test -n "$CONFIG_SND_PDAUDIOCF"; then
  AC_DEFINE(CONFIG_SND_PDAUDIOCF_MODULE)
fi
if test -n "$CONFIG_SND_SPARC"; then
  AC_DEFINE(CONFIG_SND_SPARC)
fi
if test -n "$CONFIG_SND_SUN_AMD7930"; then
  AC_DEFINE(CONFIG_SND_SUN_AMD7930_MODULE)
fi
if test -n "$CONFIG_SND_SUN_CS4231"; then
  AC_DEFINE(CONFIG_SND_SUN_CS4231_MODULE)
fi
if test -n "$CONFIG_SND_SUN_DBRI"; then
  AC_DEFINE(CONFIG_SND_SUN_DBRI_MODULE)
fi
if test -n "$CONFIG_SND_GSC"; then
  AC_DEFINE(CONFIG_SND_GSC)
fi
if test -n "$CONFIG_SND_HARMONY"; then
  AC_DEFINE(CONFIG_SND_HARMONY_MODULE)
fi
if test -n "$CONFIG_SND_SOC"; then
  AC_DEFINE(CONFIG_SND_SOC_MODULE)
fi
if test -n "$CONFIG_SND_SOC_AC97_BUS"; then
  AC_DEFINE(CONFIG_SND_SOC_AC97_BUS)
fi
if test -n "$CONFIG_SND_AT32_SOC"; then
  AC_DEFINE(CONFIG_SND_AT32_SOC_MODULE)
fi
if test -n "$CONFIG_SND_AT32_SOC_SSC"; then
  AC_DEFINE(CONFIG_SND_AT32_SOC_SSC_MODULE)
fi
if test -n "$CONFIG_SND_AT32_SOC_PLAYPAQ"; then
  AC_DEFINE(CONFIG_SND_AT32_SOC_PLAYPAQ_MODULE)
fi
if test -n "$CONFIG_SND_AT32_SOC_PLAYPAQ_SLAVE"; then
  AC_DEFINE(CONFIG_SND_AT32_SOC_PLAYPAQ_SLAVE)
fi
if test -n "$CONFIG_SND_AT91_SOC"; then
  AC_DEFINE(CONFIG_SND_AT91_SOC_MODULE)
fi
if test -n "$CONFIG_SND_AT91_SOC_SSC"; then
  AC_DEFINE(CONFIG_SND_AT91_SOC_SSC_MODULE)
fi
if test -n "$CONFIG_SND_SOC_AU1XPSC"; then
  AC_DEFINE(CONFIG_SND_SOC_AU1XPSC_MODULE)
fi
if test -n "$CONFIG_SND_SOC_AU1XPSC_I2S"; then
  AC_DEFINE(CONFIG_SND_SOC_AU1XPSC_I2S_MODULE)
fi
if test -n "$CONFIG_SND_SOC_AU1XPSC_AC97"; then
  AC_DEFINE(CONFIG_SND_SOC_AU1XPSC_AC97_MODULE)
fi
if test -n "$CONFIG_SND_SOC_SAMPLE_PSC_AC97"; then
  AC_DEFINE(CONFIG_SND_SOC_SAMPLE_PSC_AC97_MODULE)
fi
if test -n "$CONFIG_SND_PXA2XX_SOC"; then
  AC_DEFINE(CONFIG_SND_PXA2XX_SOC_MODULE)
fi
if test -n "$CONFIG_SND_PXA2XX_SOC_AC97"; then
  AC_DEFINE(CONFIG_SND_PXA2XX_SOC_AC97_MODULE)
fi
if test -n "$CONFIG_SND_PXA2XX_SOC_I2S"; then
  AC_DEFINE(CONFIG_SND_PXA2XX_SOC_I2S_MODULE)
fi
if test -n "$CONFIG_SND_PXA2XX_SOC_CORGI"; then
  AC_DEFINE(CONFIG_SND_PXA2XX_SOC_CORGI_MODULE)
fi
if test -n "$CONFIG_SND_PXA2XX_SOC_SPITZ"; then
  AC_DEFINE(CONFIG_SND_PXA2XX_SOC_SPITZ_MODULE)
fi
if test -n "$CONFIG_SND_PXA2XX_SOC_POODLE"; then
  AC_DEFINE(CONFIG_SND_PXA2XX_SOC_POODLE_MODULE)
fi
if test -n "$CONFIG_SND_PXA2XX_SOC_TOSA"; then
  AC_DEFINE(CONFIG_SND_PXA2XX_SOC_TOSA_MODULE)
fi
if test -n "$CONFIG_SND_PXA2XX_SOC_E800"; then
  AC_DEFINE(CONFIG_SND_PXA2XX_SOC_E800_MODULE)
fi
if test -n "$CONFIG_SND_PXA2XX_SOC_EM_X270"; then
  AC_DEFINE(CONFIG_SND_PXA2XX_SOC_EM_X270_MODULE)
fi
if test -n "$CONFIG_SND_S3C24XX_SOC"; then
  AC_DEFINE(CONFIG_SND_S3C24XX_SOC_MODULE)
fi
if test -n "$CONFIG_SND_S3C24XX_SOC_I2S"; then
  AC_DEFINE(CONFIG_SND_S3C24XX_SOC_I2S_MODULE)
fi
if test -n "$CONFIG_SND_S3C2412_SOC_I2S"; then
  AC_DEFINE(CONFIG_SND_S3C2412_SOC_I2S_MODULE)
fi
if test -n "$CONFIG_SND_S3C2443_SOC_AC97"; then
  AC_DEFINE(CONFIG_SND_S3C2443_SOC_AC97_MODULE)
fi
if test -n "$CONFIG_SND_S3C24XX_SOC_NEO1973_WM8753"; then
  AC_DEFINE(CONFIG_SND_S3C24XX_SOC_NEO1973_WM8753_MODULE)
fi
if test -n "$CONFIG_SND_S3C24XX_SOC_SMDK2443_WM9710"; then
  AC_DEFINE(CONFIG_SND_S3C24XX_SOC_SMDK2443_WM9710_MODULE)
fi
if test -n "$CONFIG_SND_S3C24XX_SOC_LN2440SBC_ALC650"; then
  AC_DEFINE(CONFIG_SND_S3C24XX_SOC_LN2440SBC_ALC650_MODULE)
fi
if test -n "$CONFIG_SND_SOC_PCM_SH7760"; then
  AC_DEFINE(CONFIG_SND_SOC_PCM_SH7760_MODULE)
fi
if test -n "$CONFIG_SND_SOC_SH4_HAC"; then
  AC_DEFINE(CONFIG_SND_SOC_SH4_HAC_MODULE)
fi
if test -n "$CONFIG_SND_SOC_SH4_SSI"; then
  AC_DEFINE(CONFIG_SND_SOC_SH4_SSI_MODULE)
fi
if test -n "$CONFIG_SND_SH7760_AC97"; then
  AC_DEFINE(CONFIG_SND_SH7760_AC97_MODULE)
fi
if test -n "$CONFIG_SND_SOC_OF_SIMPLE"; then
  AC_DEFINE(CONFIG_SND_SOC_OF_SIMPLE_MODULE)
fi
if test -n "$CONFIG_SND_SOC_MPC8610"; then
  AC_DEFINE(CONFIG_SND_SOC_MPC8610)
fi
if test -n "$CONFIG_SND_SOC_MPC8610_HPCD"; then
  AC_DEFINE(CONFIG_SND_SOC_MPC8610_HPCD)
fi
if test -n "$CONFIG_SND_SOC_MPC5200_I2S"; then
  AC_DEFINE(CONFIG_SND_SOC_MPC5200_I2S_MODULE)
fi
if test -n "$CONFIG_SND_DAVINCI_SOC"; then
  AC_DEFINE(CONFIG_SND_DAVINCI_SOC_MODULE)
fi
if test -n "$CONFIG_SND_DAVINCI_SOC_I2S"; then
  AC_DEFINE(CONFIG_SND_DAVINCI_SOC_I2S_MODULE)
fi
if test -n "$CONFIG_SND_DAVINCI_SOC_EVM"; then
  AC_DEFINE(CONFIG_SND_DAVINCI_SOC_EVM_MODULE)
fi
if test -n "$CONFIG_SND_OMAP_SOC"; then
  AC_DEFINE(CONFIG_SND_OMAP_SOC_MODULE)
fi
if test -n "$CONFIG_SND_OMAP_SOC_MCBSP"; then
  AC_DEFINE(CONFIG_SND_OMAP_SOC_MCBSP_MODULE)
fi
if test -n "$CONFIG_SND_OMAP_SOC_N810"; then
  AC_DEFINE(CONFIG_SND_OMAP_SOC_N810_MODULE)
fi
if test -n "$CONFIG_SND_OMAP_SOC_OSK5912"; then
  AC_DEFINE(CONFIG_SND_OMAP_SOC_OSK5912_MODULE)
fi
if test -n "$CONFIG_SND_BF5XX_I2S"; then
  AC_DEFINE(CONFIG_SND_BF5XX_I2S_MODULE)
fi
if test -n "$CONFIG_SND_BF5XX_SOC_SSM2602"; then
  AC_DEFINE(CONFIG_SND_BF5XX_SOC_SSM2602_MODULE)
fi
if test -n "$CONFIG_SND_BF5XX_SOC_AD73311"; then
  AC_DEFINE(CONFIG_SND_BF5XX_SOC_AD73311_MODULE)
fi
if test -n "$CONFIG_SND_BFIN_AD73311_SE"; then
  AC_DEFINE_UNQUOTED([CONFIG_SND_BFIN_AD73311_SE], [4])
fi
if test -n "$CONFIG_SND_BF5XX_AC97"; then
  AC_DEFINE(CONFIG_SND_BF5XX_AC97_MODULE)
fi
if test -n "$CONFIG_SND_MMAP_SUPPORT"; then
  AC_DEFINE(CONFIG_SND_MMAP_SUPPORT)
fi
if test -n "$CONFIG_SND_BF5XX_SOC_SPORT"; then
  AC_DEFINE(CONFIG_SND_BF5XX_SOC_SPORT_MODULE)
fi
if test -n "$CONFIG_SND_BF5XX_SOC_I2S"; then
  AC_DEFINE(CONFIG_SND_BF5XX_SOC_I2S_MODULE)
fi
if test -n "$CONFIG_SND_BF5XX_SOC_AC97"; then
  AC_DEFINE(CONFIG_SND_BF5XX_SOC_AC97_MODULE)
fi
if test -n "$CONFIG_SND_BF5XX_SOC_AD1980"; then
  AC_DEFINE(CONFIG_SND_BF5XX_SOC_AD1980_MODULE)
fi
if test -n "$CONFIG_SND_BF5XX_SPORT_NUM"; then
  AC_DEFINE_UNQUOTED([CONFIG_SND_BF5XX_SPORT_NUM], [0])
fi
if test -n "$CONFIG_SND_BF5XX_HAVE_COLD_RESET"; then
  AC_DEFINE(CONFIG_SND_BF5XX_HAVE_COLD_RESET)
fi
if test -n "$CONFIG_SND_BF5XX_RESET_GPIO_NUM"; then
  AC_DEFINE_UNQUOTED([CONFIG_SND_BF5XX_RESET_GPIO_NUM], [5])
fi
if test -n "$CONFIG_SND_SOC_ALL_CODECS"; then
  AC_DEFINE(CONFIG_SND_SOC_ALL_CODECS_MODULE)
fi
if test -n "$CONFIG_SND_SOC_AC97_CODEC"; then
  AC_DEFINE(CONFIG_SND_SOC_AC97_CODEC_MODULE)
fi
if test -n "$CONFIG_SND_SOC_AD1980"; then
  AC_DEFINE(CONFIG_SND_SOC_AD1980_MODULE)
fi
if test -n "$CONFIG_SND_SOC_AD73311"; then
  AC_DEFINE(CONFIG_SND_SOC_AD73311_MODULE)
fi
if test -n "$CONFIG_SND_SOC_AK4535"; then
  AC_DEFINE(CONFIG_SND_SOC_AK4535_MODULE)
fi
if test -n "$CONFIG_SND_SOC_CS4270"; then
  AC_DEFINE(CONFIG_SND_SOC_CS4270_MODULE)
fi
if test -n "$CONFIG_SND_SOC_CS4270_HWMUTE"; then
  AC_DEFINE(CONFIG_SND_SOC_CS4270_HWMUTE)
fi
if test -n "$CONFIG_SND_SOC_CS4270_VD33_ERRATA"; then
  AC_DEFINE(CONFIG_SND_SOC_CS4270_VD33_ERRATA)
fi
if test -n "$CONFIG_SND_SOC_SSM2602"; then
  AC_DEFINE(CONFIG_SND_SOC_SSM2602_MODULE)
fi
if test -n "$CONFIG_SND_SOC_TLV320AIC23"; then
  AC_DEFINE(CONFIG_SND_SOC_TLV320AIC23_MODULE)
fi
if test -n "$CONFIG_SND_SOC_TLV320AIC26"; then
  AC_DEFINE(CONFIG_SND_SOC_TLV320AIC26_MODULE)
fi
if test -n "$CONFIG_SND_SOC_TLV320AIC3X"; then
  AC_DEFINE(CONFIG_SND_SOC_TLV320AIC3X_MODULE)
fi
if test -n "$CONFIG_SND_SOC_UDA1380"; then
  AC_DEFINE(CONFIG_SND_SOC_UDA1380_MODULE)
fi
if test -n "$CONFIG_SND_SOC_WM8510"; then
  AC_DEFINE(CONFIG_SND_SOC_WM8510_MODULE)
fi
if test -n "$CONFIG_SND_SOC_WM8580"; then
  AC_DEFINE(CONFIG_SND_SOC_WM8580_MODULE)
fi
if test -n "$CONFIG_SND_SOC_WM8731"; then
  AC_DEFINE(CONFIG_SND_SOC_WM8731_MODULE)
fi
if test -n "$CONFIG_SND_SOC_WM8750"; then
  AC_DEFINE(CONFIG_SND_SOC_WM8750_MODULE)
fi
if test -n "$CONFIG_SND_SOC_WM8753"; then
  AC_DEFINE(CONFIG_SND_SOC_WM8753_MODULE)
fi
if test -n "$CONFIG_SND_SOC_WM8900"; then
  AC_DEFINE(CONFIG_SND_SOC_WM8900_MODULE)
fi
if test -n "$CONFIG_SND_SOC_WM8903"; then
  AC_DEFINE(CONFIG_SND_SOC_WM8903_MODULE)
fi
if test -n "$CONFIG_SND_SOC_WM8971"; then
  AC_DEFINE(CONFIG_SND_SOC_WM8971_MODULE)
fi
if test -n "$CONFIG_SND_SOC_WM8990"; then
  AC_DEFINE(CONFIG_SND_SOC_WM8990_MODULE)
fi
if test -n "$CONFIG_SND_SOC_WM9712"; then
  AC_DEFINE(CONFIG_SND_SOC_WM9712_MODULE)
fi
if test -n "$CONFIG_SND_SOC_WM9713"; then
  AC_DEFINE(CONFIG_SND_SOC_WM9713_MODULE)
fi
if test -n "$CONFIG_SOUND_PRIME"; then
  AC_DEFINE(CONFIG_SOUND_PRIME_MODULE)
fi
if test -n "$CONFIG_AC97_BUS"; then
  AC_DEFINE(CONFIG_AC97_BUS_MODULE)
fi
])

AC_DEFUN([ALSA_TOPLEVEL_OUTPUT], [
dnl output all subst
AC_SUBST(CONFIG_SOUND)
AC_SUBST(CONFIG_HAS_IOMEM)
AC_SUBST(CONFIG_SOUND_OSS_CORE)
AC_SUBST(CONFIG_M68K)
AC_SUBST(CONFIG_SND)
AC_SUBST(CONFIG_SND_TIMER)
AC_SUBST(CONFIG_SND_PCM)
AC_SUBST(CONFIG_SND_HWDEP)
AC_SUBST(CONFIG_SND_RAWMIDI)
AC_SUBST(CONFIG_SND_JACK)
AC_SUBST(CONFIG_SND_SEQUENCER)
AC_SUBST(CONFIG_SND_SEQ_DUMMY)
AC_SUBST(CONFIG_SND_OSSEMUL)
AC_SUBST(CONFIG_SND_MIXER_OSS)
AC_SUBST(CONFIG_SND_PCM_OSS)
AC_SUBST(CONFIG_SND_PCM_OSS_PLUGINS)
AC_SUBST(CONFIG_SND_SEQUENCER_OSS)
AC_SUBST(CONFIG_SND_RTCTIMER)
AC_SUBST(CONFIG_RTC)
AC_SUBST(CONFIG_SND_SEQ_RTCTIMER_DEFAULT)
AC_SUBST(CONFIG_SND_DYNAMIC_MINORS)
AC_SUBST(CONFIG_SND_SUPPORT_OLD_API)
AC_SUBST(CONFIG_SND_VERBOSE_PROCFS)
AC_SUBST(CONFIG_PROC_FS)
AC_SUBST(CONFIG_SND_VERBOSE_PRINTK)
AC_SUBST(CONFIG_SND_DEBUG)
AC_SUBST(CONFIG_SND_DEBUG_VERBOSE)
AC_SUBST(CONFIG_SND_PCM_XRUN_DEBUG)
AC_SUBST(CONFIG_SND_VMASTER)
AC_SUBST(CONFIG_SND_BIT32_EMUL)
AC_SUBST(CONFIG_SND_DEBUG_MEMORY)
AC_SUBST(CONFIG_SND_HPET)
AC_SUBST(CONFIG_HPET)
AC_SUBST(CONFIG_BROKEN)
AC_SUBST(CONFIG_SND_MPU401_UART)
AC_SUBST(CONFIG_SND_OPL3_LIB)
AC_SUBST(CONFIG_SND_OPL4_LIB)
AC_SUBST(CONFIG_SND_VX_LIB)
AC_SUBST(CONFIG_SND_AC97_CODEC)
AC_SUBST(CONFIG_SND_DRIVERS)
AC_SUBST(CONFIG_SND_PCSP)
AC_SUBST(CONFIG_PCSPKR_PLATFORM)
AC_SUBST(CONFIG_X86_PC)
AC_SUBST(CONFIG_HIGH_RES_TIMERS)
AC_SUBST(CONFIG_INPUT)
AC_SUBST(CONFIG_EXPERIMENTAL)
AC_SUBST(CONFIG_SND_DUMMY)
AC_SUBST(CONFIG_SND_VIRMIDI)
AC_SUBST(CONFIG_SND_MTPAV)
AC_SUBST(CONFIG_SND_MTS64)
AC_SUBST(CONFIG_PARPORT)
AC_SUBST(CONFIG_SND_SERIAL_U16550)
AC_SUBST(CONFIG_SND_MPU401)
AC_SUBST(CONFIG_SND_PORTMAN2X4)
AC_SUBST(CONFIG_SND_ML403_AC97CR)
AC_SUBST(CONFIG_XILINX_VIRTEX)
AC_SUBST(CONFIG_SND_AC97_POWER_SAVE)
AC_SUBST(CONFIG_SND_AC97_POWER_SAVE_DEFAULT)
AC_SUBST(CONFIG_SND_SERIALMIDI)
AC_SUBST(CONFIG_SND_LOOPBACK)
AC_SUBST(CONFIG_SND_WSS_LIB)
AC_SUBST(CONFIG_SND_SB_COMMON)
AC_SUBST(CONFIG_SND_SB8_DSP)
AC_SUBST(CONFIG_SND_SB16_DSP)
AC_SUBST(CONFIG_SND_ISA)
AC_SUBST(CONFIG_ISA)
AC_SUBST(CONFIG_ISA_DMA_API)
AC_SUBST(CONFIG_SND_ADLIB)
AC_SUBST(CONFIG_SND_AD1816A)
AC_SUBST(CONFIG_PNP)
AC_SUBST(CONFIG_ISAPNP)
AC_SUBST(CONFIG_SND_AD1848)
AC_SUBST(CONFIG_SND_ALS100)
AC_SUBST(CONFIG_SND_AZT2320)
AC_SUBST(CONFIG_SND_CMI8330)
AC_SUBST(CONFIG_SND_CS4231)
AC_SUBST(CONFIG_SND_CS4232)
AC_SUBST(CONFIG_SND_CS4236)
AC_SUBST(CONFIG_SND_DT019X)
AC_SUBST(CONFIG_SND_ES968)
AC_SUBST(CONFIG_SND_ES1688)
AC_SUBST(CONFIG_SND_ES18XX)
AC_SUBST(CONFIG_SND_SC6000)
AC_SUBST(CONFIG_HAS_IOPORT)
AC_SUBST(CONFIG_SND_GUSCLASSIC)
AC_SUBST(CONFIG_SND_GUSEXTREME)
AC_SUBST(CONFIG_SND_GUSMAX)
AC_SUBST(CONFIG_SND_INTERWAVE)
AC_SUBST(CONFIG_SND_INTERWAVE_STB)
AC_SUBST(CONFIG_SND_OPL3SA2)
AC_SUBST(CONFIG_SND_OPTI92X_AD1848)
AC_SUBST(CONFIG_SND_OPTI92X_CS4231)
AC_SUBST(CONFIG_SND_OPTI93X)
AC_SUBST(CONFIG_SND_MIRO)
AC_SUBST(CONFIG_SND_SB8)
AC_SUBST(CONFIG_SND_SB16)
AC_SUBST(CONFIG_SND_SBAWE)
AC_SUBST(CONFIG_SND_SB16_CSP)
AC_SUBST(CONFIG_PPC)
AC_SUBST(CONFIG_FW_LOADER)
AC_SUBST(CONFIG_SND_SGALAXY)
AC_SUBST(CONFIG_SND_SSCAPE)
AC_SUBST(CONFIG_SND_WAVEFRONT)
AC_SUBST(CONFIG_SND_WAVEFRONT_FIRMWARE_IN_KERNEL)
AC_SUBST(CONFIG_SND_PC98_CS4232)
AC_SUBST(CONFIG_X86_PC9800)
AC_SUBST(CONFIG_SND_MSND_PINNACLE)
AC_SUBST(CONFIG_X86)
AC_SUBST(CONFIG_SND_PCI)
AC_SUBST(CONFIG_PCI)
AC_SUBST(CONFIG_SND_AD1889)
AC_SUBST(CONFIG_SND_ALS300)
AC_SUBST(CONFIG_SND_ALS4000)
AC_SUBST(CONFIG_SND_ALI5451)
AC_SUBST(CONFIG_SND_ATIIXP)
AC_SUBST(CONFIG_SND_ATIIXP_MODEM)
AC_SUBST(CONFIG_SND_AU8810)
AC_SUBST(CONFIG_SND_AU8820)
AC_SUBST(CONFIG_SND_AU8830)
AC_SUBST(CONFIG_SND_AW2)
AC_SUBST(CONFIG_SND_AZT3328)
AC_SUBST(CONFIG_SND_BT87X)
AC_SUBST(CONFIG_SND_BT87X_OVERCLOCK)
AC_SUBST(CONFIG_SND_CA0106)
AC_SUBST(CONFIG_SND_CMIPCI)
AC_SUBST(CONFIG_SND_OXYGEN_LIB)
AC_SUBST(CONFIG_SND_OXYGEN)
AC_SUBST(CONFIG_SND_CS4281)
AC_SUBST(CONFIG_SND_CS46XX)
AC_SUBST(CONFIG_SND_CS46XX_NEW_DSP)
AC_SUBST(CONFIG_SND_CS5530)
AC_SUBST(CONFIG_SND_CS5535AUDIO)
AC_SUBST(CONFIG_X86_64)
AC_SUBST(CONFIG_SND_DARLA20)
AC_SUBST(CONFIG_SND_GINA20)
AC_SUBST(CONFIG_SND_LAYLA20)
AC_SUBST(CONFIG_SND_DARLA24)
AC_SUBST(CONFIG_SND_GINA24)
AC_SUBST(CONFIG_SND_LAYLA24)
AC_SUBST(CONFIG_SND_MONA)
AC_SUBST(CONFIG_SND_MIA)
AC_SUBST(CONFIG_SND_ECHO3G)
AC_SUBST(CONFIG_SND_INDIGO)
AC_SUBST(CONFIG_SND_INDIGOIO)
AC_SUBST(CONFIG_SND_INDIGODJ)
AC_SUBST(CONFIG_SND_EMU10K1)
AC_SUBST(CONFIG_SND_EMU10K1X)
AC_SUBST(CONFIG_SND_ENS1370)
AC_SUBST(CONFIG_SND_ENS1371)
AC_SUBST(CONFIG_SND_ES1938)
AC_SUBST(CONFIG_SND_ES1968)
AC_SUBST(CONFIG_SND_FM801)
AC_SUBST(CONFIG_SND_FM801_TEA575X_BOOL)
AC_SUBST(CONFIG_VIDEO_V4L1)
AC_SUBST(CONFIG_SND_FM801_TEA575X)
AC_SUBST(CONFIG_SND_HDA_INTEL)
AC_SUBST(CONFIG_SND_HDA_HWDEP)
AC_SUBST(CONFIG_SND_HDA_INPUT_BEEP)
AC_SUBST(CONFIG_SND_HDA_CODEC_REALTEK)
AC_SUBST(CONFIG_SND_HDA_CODEC_ANALOG)
AC_SUBST(CONFIG_SND_HDA_CODEC_SIGMATEL)
AC_SUBST(CONFIG_SND_HDA_CODEC_VIA)
AC_SUBST(CONFIG_SND_HDA_CODEC_ATIHDMI)
AC_SUBST(CONFIG_SND_HDA_CODEC_NVHDMI)
AC_SUBST(CONFIG_SND_HDA_CODEC_CONEXANT)
AC_SUBST(CONFIG_SND_HDA_CODEC_CMEDIA)
AC_SUBST(CONFIG_SND_HDA_CODEC_SI3054)
AC_SUBST(CONFIG_SND_HDA_GENERIC)
AC_SUBST(CONFIG_SND_HDA_POWER_SAVE)
AC_SUBST(CONFIG_SND_HDA_POWER_SAVE_DEFAULT)
AC_SUBST(CONFIG_SND_HDSP)
AC_SUBST(CONFIG_SND_HDSPM)
AC_SUBST(CONFIG_SND_HIFIER)
AC_SUBST(CONFIG_SND_ICE1712)
AC_SUBST(CONFIG_SND_ICE1724)
AC_SUBST(CONFIG_SND_INTEL8X0)
AC_SUBST(CONFIG_SND_INTEL8X0M)
AC_SUBST(CONFIG_SND_KORG1212)
AC_SUBST(CONFIG_SND_MAESTRO3)
AC_SUBST(CONFIG_SND_MIXART)
AC_SUBST(CONFIG_SND_NM256)
AC_SUBST(CONFIG_SND_PCXHR)
AC_SUBST(CONFIG_SND_RIPTIDE)
AC_SUBST(CONFIG_SND_RME32)
AC_SUBST(CONFIG_SND_RME96)
AC_SUBST(CONFIG_SND_RME9652)
AC_SUBST(CONFIG_SND_SIS7019)
AC_SUBST(CONFIG_SND_SONICVIBES)
AC_SUBST(CONFIG_SND_TRIDENT)
AC_SUBST(CONFIG_SND_VIA82XX)
AC_SUBST(CONFIG_SND_VIA82XX_MODEM)
AC_SUBST(CONFIG_SND_VIRTUOSO)
AC_SUBST(CONFIG_SND_VX222)
AC_SUBST(CONFIG_SND_YMFPCI)
AC_SUBST(CONFIG_SND_PDPLUS)
AC_SUBST(CONFIG_SND_ASIHPI)
AC_SUBST(CONFIG_SND_PPC)
AC_SUBST(CONFIG_PPC64)
AC_SUBST(CONFIG_PPC32)
AC_SUBST(CONFIG_SND_POWERMAC)
AC_SUBST(CONFIG_I2C)
AC_SUBST(CONFIG_PPC_PMAC)
AC_SUBST(CONFIG_SND_POWERMAC_AUTO_DRC)
AC_SUBST(CONFIG_SND_PS3)
AC_SUBST(CONFIG_PS3_PS3AV)
AC_SUBST(CONFIG_SND_PS3_DEFAULT_START_DELAY)
AC_SUBST(CONFIG_SND_AOA)
AC_SUBST(CONFIG_SND_AOA_FABRIC_LAYOUT)
AC_SUBST(CONFIG_SND_AOA_ONYX)
AC_SUBST(CONFIG_I2C_POWERMAC)
AC_SUBST(CONFIG_SND_AOA_TAS)
AC_SUBST(CONFIG_SND_AOA_TOONIE)
AC_SUBST(CONFIG_SND_AOA_SOUNDBUS)
AC_SUBST(CONFIG_SND_AOA_SOUNDBUS_I2S)
AC_SUBST(CONFIG_SND_ARM)
AC_SUBST(CONFIG_ARM)
AC_SUBST(CONFIG_SND_SA11XX_UDA1341)
AC_SUBST(CONFIG_ARCH_SA1100)
AC_SUBST(CONFIG_L3)
AC_SUBST(CONFIG_SND_ARMAACI)
AC_SUBST(CONFIG_ARM_AMBA)
AC_SUBST(CONFIG_SND_PXA2XX_PCM)
AC_SUBST(CONFIG_SND_PXA2XX_LIB)
AC_SUBST(CONFIG_SND_PXA2XX_LIB_AC97)
AC_SUBST(CONFIG_SND_PXA2XX_AC97)
AC_SUBST(CONFIG_ARCH_PXA)
AC_SUBST(CONFIG_SND_S3C2410)
AC_SUBST(CONFIG_ARCH_S3C2410)
AC_SUBST(CONFIG_I2C_SENSOR)
AC_SUBST(CONFIG_SND_PXA2XX_I2SOUND)
AC_SUBST(CONFIG_SND_SPI)
AC_SUBST(CONFIG_SPI)
AC_SUBST(CONFIG_SND_AT73C213)
AC_SUBST(CONFIG_ATMEL_SSC)
AC_SUBST(CONFIG_SND_AT73C213_TARGET_BITRATE)
AC_SUBST(CONFIG_SND_MIPS)
AC_SUBST(CONFIG_MIPS)
AC_SUBST(CONFIG_SND_SGI_O2)
AC_SUBST(CONFIG_SGI_IP32)
AC_SUBST(CONFIG_SND_SGI_HAL2)
AC_SUBST(CONFIG_SGI_HAS_HAL2)
AC_SUBST(CONFIG_SND_AU1X00)
AC_SUBST(CONFIG_SOC_AU1000)
AC_SUBST(CONFIG_SOC_AU1100)
AC_SUBST(CONFIG_SOC_AU1500)
AC_SUBST(CONFIG_SND_SUPERH)
AC_SUBST(CONFIG_SUPERH)
AC_SUBST(CONFIG_SND_AICA)
AC_SUBST(CONFIG_SH_DREAMCAST)
AC_SUBST(CONFIG_SND_USB)
AC_SUBST(CONFIG_USB)
AC_SUBST(CONFIG_SND_USB_AUDIO)
AC_SUBST(CONFIG_SND_USB_USX2Y)
AC_SUBST(CONFIG_ALPHA)
AC_SUBST(CONFIG_SND_USB_CAIAQ)
AC_SUBST(CONFIG_SND_USB_CAIAQ_INPUT)
AC_SUBST(CONFIG_SND_USB_US122L)
AC_SUBST(CONFIG_SND_PCMCIA)
AC_SUBST(CONFIG_PCMCIA)
AC_SUBST(CONFIG_SND_VXPOCKET)
AC_SUBST(CONFIG_SND_PDAUDIOCF)
AC_SUBST(CONFIG_SND_SPARC)
AC_SUBST(CONFIG_SPARC)
AC_SUBST(CONFIG_SND_SUN_AMD7930)
AC_SUBST(CONFIG_SBUS)
AC_SUBST(CONFIG_SND_SUN_CS4231)
AC_SUBST(CONFIG_SND_SUN_DBRI)
AC_SUBST(CONFIG_SND_GSC)
AC_SUBST(CONFIG_GSC)
AC_SUBST(CONFIG_SND_HARMONY)
AC_SUBST(CONFIG_SND_SOC)
AC_SUBST(CONFIG_SND_SOC_AC97_BUS)
AC_SUBST(CONFIG_SND_AT32_SOC)
AC_SUBST(CONFIG_AVR32)
AC_SUBST(CONFIG_SND_AT32_SOC_SSC)
AC_SUBST(CONFIG_SND_AT32_SOC_PLAYPAQ)
AC_SUBST(CONFIG_BOARD_PLAYPAQ)
AC_SUBST(CONFIG_SND_AT32_SOC_PLAYPAQ_SLAVE)
AC_SUBST(CONFIG_SND_AT91_SOC)
AC_SUBST(CONFIG_ARCH_AT91)
AC_SUBST(CONFIG_SND_AT91_SOC_SSC)
AC_SUBST(CONFIG_SND_SOC_AU1XPSC)
AC_SUBST(CONFIG_SOC_AU1200)
AC_SUBST(CONFIG_SOC_AU1550)
AC_SUBST(CONFIG_SND_SOC_AU1XPSC_I2S)
AC_SUBST(CONFIG_SND_SOC_AU1XPSC_AC97)
AC_SUBST(CONFIG_SND_SOC_SAMPLE_PSC_AC97)
AC_SUBST(CONFIG_SND_PXA2XX_SOC)
AC_SUBST(CONFIG_SND_PXA2XX_SOC_AC97)
AC_SUBST(CONFIG_SND_PXA2XX_SOC_I2S)
AC_SUBST(CONFIG_SND_PXA2XX_SOC_CORGI)
AC_SUBST(CONFIG_PXA_SHARP_C7XX)
AC_SUBST(CONFIG_SND_PXA2XX_SOC_SPITZ)
AC_SUBST(CONFIG_PXA_SHARP_CXX00)
AC_SUBST(CONFIG_SND_PXA2XX_SOC_POODLE)
AC_SUBST(CONFIG_MACH_POODLE)
AC_SUBST(CONFIG_SND_PXA2XX_SOC_TOSA)
AC_SUBST(CONFIG_MACH_TOSA)
AC_SUBST(CONFIG_MFD_TC6393XB)
AC_SUBST(CONFIG_SND_PXA2XX_SOC_E800)
AC_SUBST(CONFIG_MACH_E800)
AC_SUBST(CONFIG_SND_PXA2XX_SOC_EM_X270)
AC_SUBST(CONFIG_MACH_EM_X270)
AC_SUBST(CONFIG_SND_S3C24XX_SOC)
AC_SUBST(CONFIG_SND_S3C24XX_SOC_I2S)
AC_SUBST(CONFIG_SND_S3C2412_SOC_I2S)
AC_SUBST(CONFIG_SND_S3C2443_SOC_AC97)
AC_SUBST(CONFIG_SND_S3C24XX_SOC_NEO1973_WM8753)
AC_SUBST(CONFIG_MACH_NEO1973_GTA01)
AC_SUBST(CONFIG_SND_S3C24XX_SOC_SMDK2443_WM9710)
AC_SUBST(CONFIG_MACH_SMDK2443)
AC_SUBST(CONFIG_SND_S3C24XX_SOC_LN2440SBC_ALC650)
AC_SUBST(CONFIG_SND_SOC_PCM_SH7760)
AC_SUBST(CONFIG_CPU_SUBTYPE_SH7760)
AC_SUBST(CONFIG_SH_DMABRG)
AC_SUBST(CONFIG_SND_SOC_SH4_HAC)
AC_SUBST(CONFIG_SND_SOC_SH4_SSI)
AC_SUBST(CONFIG_SND_SH7760_AC97)
AC_SUBST(CONFIG_SND_SOC_OF_SIMPLE)
AC_SUBST(CONFIG_SND_SOC_MPC8610)
AC_SUBST(CONFIG_MPC8610_HPCD)
AC_SUBST(CONFIG_SND_SOC_MPC8610_HPCD)
AC_SUBST(CONFIG_SND_SOC_MPC5200_I2S)
AC_SUBST(CONFIG_PPC_MPC52XX)
AC_SUBST(CONFIG_SND_DAVINCI_SOC)
AC_SUBST(CONFIG_ARCH_DAVINCI)
AC_SUBST(CONFIG_SND_DAVINCI_SOC_I2S)
AC_SUBST(CONFIG_SND_DAVINCI_SOC_EVM)
AC_SUBST(CONFIG_MACH_DAVINCI_EVM)
AC_SUBST(CONFIG_SND_OMAP_SOC)
AC_SUBST(CONFIG_ARCH_OMAP)
AC_SUBST(CONFIG_SND_OMAP_SOC_MCBSP)
AC_SUBST(CONFIG_OMAP_MCBSP)
AC_SUBST(CONFIG_SND_OMAP_SOC_N810)
AC_SUBST(CONFIG_MACH_NOKIA_N810)
AC_SUBST(CONFIG_SND_OMAP_SOC_OSK5912)
AC_SUBST(CONFIG_MACH_OMAP_OSK)
AC_SUBST(CONFIG_SND_BF5XX_I2S)
AC_SUBST(CONFIG_BLACKFIN)
AC_SUBST(CONFIG_SND_BF5XX_SOC_SSM2602)
AC_SUBST(CONFIG_I2C_BLACKFIN_TWI)
AC_SUBST(CONFIG_SND_BF5XX_SOC_AD73311)
AC_SUBST(CONFIG_SND_BFIN_AD73311_SE)
AC_SUBST(CONFIG_SND_BF5XX_AC97)
AC_SUBST(CONFIG_SND_MMAP_SUPPORT)
AC_SUBST(CONFIG_SND_BF5XX_SOC_SPORT)
AC_SUBST(CONFIG_SND_BF5XX_SOC_I2S)
AC_SUBST(CONFIG_SND_BF5XX_SOC_AC97)
AC_SUBST(CONFIG_SND_BF5XX_SOC_AD1980)
AC_SUBST(CONFIG_SND_BF5XX_SPORT_NUM)
AC_SUBST(CONFIG_SND_BF5XX_HAVE_COLD_RESET)
AC_SUBST(CONFIG_SND_BF5XX_RESET_GPIO_NUM)
AC_SUBST(CONFIG_SND_SOC_ALL_CODECS)
AC_SUBST(CONFIG_SPI_MASTER)
AC_SUBST(CONFIG_SND_SOC_AC97_CODEC)
AC_SUBST(CONFIG_SND_SOC_AD1980)
AC_SUBST(CONFIG_SND_SOC_AD73311)
AC_SUBST(CONFIG_SND_SOC_AK4535)
AC_SUBST(CONFIG_SND_SOC_CS4270)
AC_SUBST(CONFIG_SND_SOC_CS4270_HWMUTE)
AC_SUBST(CONFIG_SND_SOC_CS4270_VD33_ERRATA)
AC_SUBST(CONFIG_SND_SOC_SSM2602)
AC_SUBST(CONFIG_SND_SOC_TLV320AIC23)
AC_SUBST(CONFIG_SND_SOC_TLV320AIC26)
AC_SUBST(CONFIG_SND_SOC_TLV320AIC3X)
AC_SUBST(CONFIG_SND_SOC_UDA1380)
AC_SUBST(CONFIG_SND_SOC_WM8510)
AC_SUBST(CONFIG_SND_SOC_WM8580)
AC_SUBST(CONFIG_SND_SOC_WM8731)
AC_SUBST(CONFIG_SND_SOC_WM8750)
AC_SUBST(CONFIG_SND_SOC_WM8753)
AC_SUBST(CONFIG_SND_SOC_WM8900)
AC_SUBST(CONFIG_SND_SOC_WM8903)
AC_SUBST(CONFIG_SND_SOC_WM8971)
AC_SUBST(CONFIG_SND_SOC_WM8990)
AC_SUBST(CONFIG_SND_SOC_WM9712)
AC_SUBST(CONFIG_SND_SOC_WM9713)
AC_SUBST(CONFIG_SOUND_PRIME)
AC_SUBST(CONFIG_AC97_BUS)
])

