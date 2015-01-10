#include "Defines.h"

#ifndef __MINGW32__
//display a 32 bit deep png image
void Png32_Show(HDC hdc,int xDest,int yDest,int nWidth,int nHeight,
       PNGINFO *pPngInfo,int xSour,int ySour)
{
 if(xSour+nWidth>(int)pPngInfo->nWidth) nWidth=pPngInfo->nWidth-xSour;
 if(ySour+nHeight>(int)pPngInfo->nHeight) nHeight=pPngInfo->nHeight-ySour;
 if(nWidth>0 && nHeight>0)
 {
 HDC hmemdc=0;
 LPBYTE pBitsDest=NULL;
 HBITMAP hbmpDest=0;
 HGDIOBJ hOldBmp=0;
 BITMAPINFO bmi;
 //sour memory
 unsigned char ** row_pointers = 
   pPngInfo->ppbyRow+pPngInfo->nHeight-1-(pPngInfo->nHeight-ySour-nHeight);
 //Do alpla blend
 int nLineTailDest=WIDTHBYTES(24*nWidth)-3*nWidth;
 // Initialize header to 0s.
 ZeroMemory(&bmi, sizeof(bmi));
 // Fill out the fields you care about.
 bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
 bmi.bmiHeader.biWidth = nWidth;
 bmi.bmiHeader.biHeight = nHeight;
 bmi.bmiHeader.biPlanes = 1;
 bmi.bmiHeader.biBitCount = 24;
 bmi.bmiHeader.biCompression = BI_RGB;
 // Create the surface.
 hmemdc=CreateCompatibleDC(hdc);
 // Get Dest Rectangle memory
 hbmpDest = CreateDIBSection(hdc, &bmi, DIB_RGB_COLORS, 
            (void **)&pBitsDest, NULL, 0);
 hOldBmp=SelectObject(hmemdc,hbmpDest);
 BitBlt(hmemdc,0,0,nWidth,nHeight,hdc,xDest,yDest,SRCCOPY);
#ifdef ASM_CORE
 __asm
 {
  push esi
  push edi
  push eax
  push ebx
  push ecx
  push edx
  push es
  push ds
  
  mov ax,ds
  mov es,ax
  mov edi,pBitsDest
  mov ecx,nHeight
beginy:
  mov ebx, row_pointers
  mov esi,[ebx]
  ;//offset picture's left margin
  mov eax,xSour
  imul eax,4  ;//4 bytes make up one pixel
  add esi,eax 
  mov eax,nWidth  
beginx:
   ;//get alpla value
   xor ebx,ebx
   mov bl,[esi+3] 
  
   ;//blue
   xor dx,dx
   mov dl,[edi];
   imul dx,bx
   add dh,[esi+2]
   mov [edi],dh ;//save result to *pBitsDest
   inc edi
   ;//green
   xor dx,dx
   mov dl,[edi];
   imul dx,bx
   add dh,[esi+1]
   mov [edi],dh ;//save result to *pBitsDest
   inc edi
   ;//red
   xor dx,dx
   mov dl,[edi];
   imul dx,bx
   add dh,[esi]
   mov [edi],dh ;//save result to *pBitsDest
   inc edi
   add esi,4
   
   dec eax
   cmp eax,0
   jne beginx
  add edi,nLineTailDest
  sub row_pointers,4 ;//next line
  loop beginy
  pop ds
  pop es
  pop edx
  pop ecx
  pop ebx
  pop eax
  pop edi
  pop esi
 } 
#else//ASM_CORE
 {
  int i,j;
  BYTE *p1=pBitsDest;
  for(i=0;i<nHeight;i++)
  {
   BYTE *p2=*(row_pointers--);
   for(j=0;j<nWidth;j++)
   {
    *p1++=((p2[3]*(*p1))>>8)+p2[2];
    *p1++=((p2[3]*(*p1))>>8)+p2[1];
    *p1++=((p2[3]*(*p1))>>8)+p2[0];
    p2+=4;
   }
   p1+=nLineTailDest;
  }
 }
#endif//ASM_CORE
 //render
 BitBlt(hdc,xDest,yDest,nWidth,nHeight,hmemdc,0,0,SRCCOPY);
 SelectObject(hmemdc,hOldBmp);
 //Free memory
 DeleteObject(hbmpDest);
 DeleteDC(hmemdc);
 }
}
#endif
