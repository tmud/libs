#ifndef __XML_H__
#define __XML_H__

#ifdef __cplusplus
extern "C" {
#endif

typedef void* xnode;
typedef void* xlist;
typedef const char* xstring;

void xmlInit();                                         // инициализация движка
void xmlDestroy();                                      // удаление всего и очистка

xnode xmlLoadMemory(const char *buffer, int size);      // загрузка из памяти
xnode xmlLoadMemoryToNode(xnode node, const char *buffer, int size);  
xnode xmlLoad(const char *filename);                    // загрузка файла в память
xnode xmlLoadToNode(xnode node, const char *filename);
void  xmlDeleteAllData();                               // освобождение xml от всех данных
xstring xmlGetLoadError();                              // последняя ошибка во время загрузки

xlist xmlRequest(xnode node, const char *request);      // запрос XPath
int   xmlGetListSize(xlist nodelist);                   // возвращает длину списка вершин
xnode xmlGetListNode(xlist nodelist, int index);        // получить вершину из списка
void  xmlFreeList(xlist nodelist);                      // освобождает список вершин

xstring xmlGetName(xnode node);                         // получить имя тега узла
xstring xmlGetAttribute(xnode node, const char *atname);// узнать аттрибут узла
int     xmlGetAttributesCount(xnode node);              // количество атрибутов вершины
xstring xmlGetAttributeName(xnode node, int index);     // имя атрибута
xstring xmlGetAttributeValue(xnode node, int index);    // значение атрибута
xstring xmlGetText(xnode node);                         // текст между открыв и закрывающими тегами
xstring xmlGetCDataText(xnode node);                    // данные CDATA у вершины

int   xmlSave(xnode node, const char *filename);

xnode xmlCreateNode(xnode parentnode, const char* nodename);
xnode xmlCreateRootNode(const char* nodename); 
void  xmlDeleteNode(xnode node);
int   xmlSetAttribute(xnode node, const char *atname, const char *value);
int   xmlSetText(xnode node, const char *text);
xnode xmlGetParent(xnode node);                         // получить родителькую вершину от текущей

xnode xmlMakeNodeCopy(xnode node, xnode parent_node);   // копирование данных

#ifdef __cplusplus
}
#endif // __cplusplus
#endif // __XML_H__
