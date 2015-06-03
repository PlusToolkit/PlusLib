/****************************************************************************
 *
 * $Id: v2u_id.h 13103 2011-05-18 22:04:14Z zhilin $
 *
 * Copyright (C) 2003-2011 Epiphan Systems Inc. All rights reserved.
 *
 * Defines vendor and product ids of VGA2USB hardware. Included by the
 * driver and by the user level code.
 *
 ****************************************************************************/

#ifndef _VGA2USB_ID_H_
#define _VGA2USB_ID_H_ 1

/**
 * Vendor and product ids.
 *
 * NOTE: if you are adding a new product ID, don't forget to update
 * V2U_PRODUCT_MAP macro below.
 */

#define EPIPHAN_VENDORID                0x5555
#define VGA2USB_VENDORID                EPIPHAN_VENDORID
#define VGA2USB_PRODID_VGA2USB          0x1110
#define VGA2USB_PRODID_KVM2USB          0x1120
#define VGA2USB_PRODID_DVI2USB          0x2222
#define VGA2USB_PRODID_VGA2USB_LR       0x3340
#define VGA2USB_PRODID_VGA2USB_HR       0x3332
#define VGA2USB_PRODID_VGA2USB_PRO      0x3333
#define VGA2USB_PRODID_VGA2USB_LR_RESPIN  0x3382
#define VGA2USB_PRODID_KVM2USB_LR_RESPIN  0x3383
#define VGA2USB_PRODID_VGA2USB_HR_RESPIN  0x3392
#define VGA2USB_PRODID_VGA2USB_PRO_RESPIN 0x33A2
#define VGA2USB_PRODID_DVI2USB_RESPIN   0x3380
#define VGA2USB_PRODID_KVM2USB_LR       0x3344
#define VGA2USB_PRODID_KVM2USB_PRO      0x3337
#define VGA2USB_PRODID_DVI2USB_SOLO     0x3411
#define VGA2USB_PRODID_DVI2USB_DUO      0x3422
#define VGA2USB_PRODID_VGA2FIFO         0x4000
#define VGA2USB_PRODID_KVM2FIFO         0x4004
#define VGA2USB_PRODID_DVI2FIFO         0x4080
#define VGA2USB_PRODID_DAVINCI1         0x5000
#define VGA2USB_PRODID_VGA2PCI          0x3A00
#define VGA2USB_PRODID_DVI2PCI          0x3B00
#define VGA2USB_PRODID_GIOCONDA         0x5100
#define VGA2USB_PRODID_ORNITHOPTER      0x5200

/**
 * Macros for detecting VGA2USB hardware
 */

#define VGA2USB_IS_VGA2USB(idVendor,idProduct,iProduct,iMfg) \
  ((idVendor)==VGA2USB_VENDORID && (idProduct)==VGA2USB_PRODID_VGA2USB && \
   ((iProduct)>0 || (iMfg)>0))

#define VGA2USB_IS_KVM2USB(idVendor,idProduct,iProduct,iMfg) \
  ((idVendor)==VGA2USB_VENDORID && (idProduct)==VGA2USB_PRODID_KVM2USB && \
   ((iProduct)>0 || (iMfg)>0))

#define VGA2USB_IS_DVI2USB(idVendor,idProduct,iProduct,iMfg) \
  ((idVendor)==VGA2USB_VENDORID && (idProduct)==VGA2USB_PRODID_DVI2USB && \
   ((iProduct)>0 || (iMfg)>0))

#define VGA2USB_IS_VGA2USB_PRO(idVendor,idProduct,iProduct,iMfg) \
  ((idVendor)==VGA2USB_VENDORID && (idProduct)==VGA2USB_PRODID_VGA2USB_PRO && \
   ((iProduct)>0 || (iMfg)>0))

#define VGA2USB_IS_VGA2USB_HR(idVendor,idProduct,iProduct,iMfg) \
  ((idVendor)==VGA2USB_VENDORID && (idProduct)==VGA2USB_PRODID_VGA2USB_HR && \
   ((iProduct)>0 || (iMfg)>0))

#define VGA2USB_IS_VGA2USB_LR(idVendor,idProduct,iProduct,iMfg) \
  ((idVendor)==VGA2USB_VENDORID && (idProduct)==VGA2USB_PRODID_VGA2USB_LR && \
   ((iProduct)>0 || (iMfg)>0))

#define VGA2USB_IS_VGA2USB_LR_RESPIN(idVendor,idProduct,iProduct,iMfg) \
  ((idVendor)==VGA2USB_VENDORID && (idProduct)==VGA2USB_PRODID_VGA2USB_LR_RESPIN && \
    ((iProduct)>0 || (iMfg)>0))

#define VGA2USB_IS_VGA2USB_HR_RESPIN(idVendor,idProduct,iProduct,iMfg) \
  ((idVendor)==VGA2USB_VENDORID && (idProduct)==VGA2USB_PRODID_VGA2USB_HR_RESPIN && \
    ((iProduct)>0 || (iMfg)>0))

#define VGA2USB_IS_VGA2USB_PRO_RESPIN(idVendor,idProduct,iProduct,iMfg) \
  ((idVendor)==VGA2USB_VENDORID && (idProduct)==VGA2USB_PRODID_VGA2USB_PRO_RESPIN && \
    ((iProduct)>0 || (iMfg)>0))

#define VGA2USB_IS_KVM2USB_LR(idVendor,idProduct,iProduct,iMfg) \
  ((idVendor)==VGA2USB_VENDORID && (idProduct)==VGA2USB_PRODID_KVM2USB_LR && \
   ((iProduct)>0 || (iMfg)>0))

#define VGA2USB_IS_KVM2USB_PRO(idVendor,idProduct,iProduct,iMfg) \
  ((idVendor)==VGA2USB_VENDORID && (idProduct)==VGA2USB_PRODID_KVM2USB_PRO && \
   ((iProduct)>0 || (iMfg)>0))

#define VGA2USB_IS_DVI2USB_SOLO(idVendor,idProduct,iProduct,iMfg) \
  ((idVendor)==VGA2USB_VENDORID && (idProduct)==VGA2USB_PRODID_DVI2USB_SOLO && \
   ((iProduct)>0 || (iMfg)>0))

#define VGA2USB_IS_DVI2USB_DUO(idVendor,idProduct,iProduct,iMfg) \
  ((idVendor)==VGA2USB_VENDORID && (idProduct)==VGA2USB_PRODID_DVI2USB_DUO && \
   ((iProduct)>0 || (iMfg)>0))

#define VGA2USB_IS_DVI2USB_RESPIN(idVendor,idProduct,iProduct,iMfg) \
  ((idVendor)==VGA2USB_VENDORID &&(idProduct)==VGA2USB_PRODID_DVI2USB_RESPIN &&\
   ((iProduct)>0 || (iMfg)>0))

#define VGA2USB_IS_KVM2USB_LR_RESPIN(idVendor,idProduct,iProduct,iMfg) \
  ((idVendor)==VGA2USB_VENDORID && (idProduct)==VGA2USB_PRODID_KVM2USB_LR_RESPIN && \
   ((iProduct)>0 || (iMfg)>0))

#define VGA2USB_IS_ANY(idVendor,idProduct,iProduct,iMfg) \
  (VGA2USB_IS_VGA2USB(idVendor,idProduct,iProduct,iMfg) || \
   VGA2USB_IS_KVM2USB(idVendor,idProduct,iProduct,iMfg) || \
   VGA2USB_IS_DVI2USB(idVendor,idProduct,iProduct,iMfg) || \
   VGA2USB_IS_DVI2USB_SOLO(idVendor,idProduct,iProduct,iMfg) || \
   VGA2USB_IS_DVI2USB_DUO(idVendor,idProduct,iProduct,iMfg) || \
   VGA2USB_IS_DVI2USB_RESPIN(idVendor,idProduct,iProduct,iMfg) || \
   VGA2USB_IS_VGA2USB_PRO(idVendor,idProduct,iProduct,iMfg) || \
   VGA2USB_IS_VGA2USB_HR(idVendor,idProduct,iProduct,iMfg) || \
   VGA2USB_IS_VGA2USB_LR(idVendor,idProduct,iProduct,iMfg) || \
   VGA2USB_IS_VGA2USB_LR_RESPIN(idVendor,idProduct,iProduct,iMfg) || \
   VGA2USB_IS_VGA2USB_HR_RESPIN(idVendor,idProduct,iProduct,iMfg) || \
   VGA2USB_IS_VGA2USB_PRO_RESPIN(idVendor,idProduct,iProduct,iMfg) || \
   VGA2USB_IS_KVM2USB_PRO(idVendor,idProduct,iProduct,iMfg) || \
   VGA2USB_IS_KVM2USB_LR(idVendor,idProduct,iProduct,iMfg)  || \
   VGA2USB_IS_KVM2USB_LR_RESPIN(idVendor,idProduct,iProduct,iMfg))

/**
 * Windows device name format. Used by user level code on Windows to open
 * a handle to the VGA2USB driver.
 */

#define VGA2USB_WIN_DEVICE_FORMAT   "EpiphanVga2usb%lu"

/**
 * This macro helps to map VGA2USB product ids into product names
 */
#define V2U_PRODUCT_MAP(map) \
  map( VGA2USB_PRODID_VGA2USB,       V2UProductVGA2USB,      "VGA2USB"     )\
  map( VGA2USB_PRODID_KVM2USB,       V2UProductKVM2USB,      "KVM2USB"     )\
  map( VGA2USB_PRODID_DVI2USB,       V2UProductDVI2USB,      "DVI2USB"     )\
  map( VGA2USB_PRODID_VGA2USB_LR,    V2UProductVGA2USBLR,    "VGA2USB LR"  )\
  map( VGA2USB_PRODID_VGA2USB_HR,    V2UProductVGA2USBHR,    "VGA2USB HR"  )\
  map( VGA2USB_PRODID_VGA2USB_PRO,   V2UProductVGA2USBPro,   "VGA2USB Pro" )\
  map( VGA2USB_PRODID_VGA2USB_LR_RESPIN,V2UProductVGA2USBLRRespin,"VGA2USB LR")\
  map( VGA2USB_PRODID_VGA2USB_HR_RESPIN,V2UProductVGA2USBHRRespin,"VGA2USB HR")\
  map( VGA2USB_PRODID_VGA2USB_PRO_RESPIN,V2UProductVGA2USBProRespin,"VGA2USB Pro")\
  map( VGA2USB_PRODID_DVI2USB_RESPIN,V2UProductDVI2USBRespin,"DVI2USB"     )\
  map( VGA2USB_PRODID_KVM2USB_LR,    V2UProductKVM2USBLR,    "KVM2USB LR"  )\
  map( VGA2USB_PRODID_KVM2USB_LR_RESPIN, V2UProductKVM2USBLRRespin, "KVM2USB LR")\
  map( VGA2USB_PRODID_KVM2USB_PRO,   V2UProductKVM2USBPro,   "KVM2USB Pro" )\
  map( VGA2USB_PRODID_DVI2USB_SOLO,  V2UProductDVI2USBSolo,  "DVI2USB Solo")\
  map( VGA2USB_PRODID_DVI2USB_DUO,   V2UProductDVI2USBDuo,   "DVI2USB Duo" )\
  map( VGA2USB_PRODID_VGA2FIFO,      V2UProductVGA2FIFO,     "VGA2FIFO"    )\
  map( VGA2USB_PRODID_KVM2FIFO,      V2UProductKVM2FIFO,     "KVMFIFO"     )\
  map( VGA2USB_PRODID_DVI2FIFO,      V2UProductDVI2FIFO,     "DVI2FIFO"    )\
  map( VGA2USB_PRODID_DAVINCI1,      V2UProductDVI2Davinci1, "DVI2Davinci" )\
  map( VGA2USB_PRODID_VGA2PCI,       V2UProductVGA2PCI,      "VGA2PCI"     )\
  map( VGA2USB_PRODID_GIOCONDA,      V2UProductGioconda,     "Gioconda"    )\
  map( VGA2USB_PRODID_DVI2PCI,       V2UProductDVI2PCI,      "DVI2PCI"     )

#endif /* _VGA2USB_ID_H_ */
