/*************************************** Copyright (c)******************************************************
** File name            :   tLib.h
** Latest modified Date :   2016-06-01
** Latest Version       :   0.1
** Descriptions         :   tinyOS所用的通用数据结构库文件。
**
**--------------------------------------------------------------------------------------------------------
** Created by           :   01课堂 lishutong
** Created date         :   2016-06-01
** Version              :   1.0
** Descriptions         :   The original version
**
**--------------------------------------------------------------------------------------------------------
** Copyright            :   版权所有，禁止用于商业用途
** Author Blog          :   http://ilishutong.com
**********************************************************************************************************/
#ifndef TLIB_H
#define TLIB_H

#include "stm32f10x.h"

typedef struct 
{
	uint32_t bitmap;
} tBitmap;

void tBitmapInit (tBitmap * bitmap);
uint32_t tBitmapPosCount (void);
void tBitmapSet (tBitmap * bitmap, uint32_t pos);
void tBitmapClear (tBitmap * bitmap, uint32_t pos);
uint32_t tBitmapGetFirstSet (tBitmap * bitmap);


typedef struct _tNode
{
    struct _tNode * preNode;
    struct _tNode * nextNode;
} tNode;

void tNodeInit (tNode * node);

typedef struct _tList
{   
    tNode headNode;
    uint32_t nodeCount;
} tList;

#define tNodeParent(node, parent, name) \
    (parent *)((uint32_t)node - (uint32_t)&((parent *)0)->name)

void tListInit (tList * list);
uint32_t tListCount (tList * list);
tNode * tListFirst (tList * list);
tNode * tListLast (tList * list);
tNode * tListPre (tList * list, tNode * node);
tNode * tListNext (tList * list, tNode * node);
void tListRemoveAll (tList * list);
void tListAddFirst (tList * list, tNode * node);
void tListAddLast (tList * list, tNode * node);
tNode * tListRemoveFirst (tList * list);
void tListInsertAfter (tList * list, tNode * nodeAfter, tNode * nodeToInsert);
void tListRemove (tList * list, tNode * node);

#endif /* TLIB_H */
