TITLE Player de sound en assembleur sur speaker
.MODEL large


.CODE
public    _playsound
public    _playsound1
public	  _convert
public 	  _SpeedPC


;-- PIXEL: Dessine un pixel dans une couleur donn‚e  ------------------
;-- D‚claration : void pixel( char *buffer, int longueur, char *tableau);

_playsound    proc far

arg buffer:dword,longueur:word,vit1:word,vit2:word,tableau:dword
push bp
mov bp,sp

push es
push di
push ds
push si

push ax
push bx
push cx
push dx


mov al,90h
out 43h,al
in al,61h
or al,03
out 61h,al

lds si,buffer	; adresse du pointeur en ds:si
les di,tableau	; adresse du pointeur en es:di

mov cx,longueur
cmp cx,0
je @@fin

dec cx
mov bx,0
cli

@@principal:
push cx

mov cx,vit1

@@bcl1:
push cx

mov al,byte ptr [ds:si + bx]

push bx
mov ah,0
mov bx,ax
mov al,byte ptr [es:di + bx]
pop bx
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
out 42h,al

mov cx,vit2
@@bcl2:
nop
loop @@bcl2

pop cx
loop @@bcl1

pop cx
inc bx
loop @@principal

sti

@@fin:

in al,61h
and al,0fch
out 61h,al
mov al,0b6h
out 43h,al

pop dx
pop cx
pop bx
pop ax

pop si
pop ds
pop di
pop es
pop bp
ret


_playsound    endp                                     ;Fin de la proc‚dure

_playsound1    proc far

arg buffer1:dword,longueur1:word,vit11:word,vit22:word
push bp
mov bp,sp

push es
push di
push ds
push si

push ax
push bx
push cx
push dx


mov al,90h
out 43h,al
in al,61h
or al,03
out 61h,al

lds si,buffer1	; adresse du pointeur en ds:si

mov cx,longueur1
cmp cx,0
je @@fina

dec cx
mov bx,0
cli

@@principala:
push cx

mov cx,vit11

@@bcl1a:
push cx

mov ah,byte ptr [ds:si + bx]

mov cx,8

@@bcl3a:
push cx

xor al,al
shl ah,1
adc al,al

out 42h,al

mov cx,vit2
@@bcl2a:
nop
loop @@bcl2a

pop cx
loop @@bcl3a

pop cx
loop @@bcl1a


pop cx
inc bx
loop @@principala

sti

@@fina:

in al,61h
and al,0fch
out 61h,al
mov al,0b6h
out 43h,al

pop dx
pop cx
pop bx
pop ax

pop si
pop ds
pop di
pop es
pop bp
ret


_playsound1    endp                                     ;Fin de la proc‚dure

_convert    proc far

arg buffers:dword,dests:dword,longs:word
push bp
mov bp,sp

push es
push di
push ds
push si

push ax
push bx
push cx
push dx

lds si,buffers	; adresse du pointeur en ds:si
les di,dests	; adresse du pointeur en es:di

mov cx,longs

dec cx
mov bx,0

@@principalb:
push cx

mov dh,byte ptr [ds:si + bx]

mov cx,8

@@bcl3b:

xor al,al
shl dh,1
sbb al,al
	
stosb

loop @@bcl3b

pop cx
inc bx
loop @@principalb

pop dx
pop cx
pop bx
pop ax

pop si
pop ds
pop di
pop es
pop bp
ret

_convert   endp                                     ;Fin de la proc‚dure

_SpeedPC    proc far

push bp
push es
push di
push ds
push si

push bx
push cx
push dx

cli
mov al,34h
out 43h,al

xor al,al
out 40h,al
out 40h,al

sti
mov cx,1000h

here:
dec cx
jnz here
cli
mov al,4h
out 43h,al
in al,40h
mov dl,al
in al,40h
mov dh,al
sti
neg dx
mov	ax,dx



pop dx
pop cx
pop bx

pop si
pop ds
pop di
pop es
pop bp
ret

_SpeedPC   endp                                     ;Fin de la proc‚dure

;== Fin ================================================================

         end                             ;Fin de la source en assembleur
