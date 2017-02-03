#ifndef __XML_H__
#define __XML_H__

#ifdef __cplusplus
extern "C" {
#endif

typedef void* xnode;
typedef void* xlist;
typedef const char* xstring;

void xmlInit();                                         // ������������� ������
void xmlDestroy();                                      // �������� ����� � �������

xnode xmlLoadMemory(const char *buffer, int size);      // �������� �� ������
xnode xmlLoadMemoryToNode(xnode node, const char *buffer, int size);  
xnode xmlLoad(const char *filename);                    // �������� ����� � ������
xnode xmlLoadToNode(xnode node, const char *filename);
void  xmlDeleteAllData();                               // ������������ xml �� ���� ������
xstring xmlGetLoadError();                              // ��������� ������ �� ����� ��������

xlist xmlRequest(xnode node, const char *request);      // ������ XPath
int   xmlGetListSize(xlist nodelist);                   // ���������� ����� ������ ������
xnode xmlGetListNode(xlist nodelist, int index);        // �������� ������� �� ������
void  xmlFreeList(xlist nodelist);                      // ����������� ������ ������

xstring xmlGetName(xnode node);                         // �������� ��� ���� ����
xstring xmlGetAttribute(xnode node, const char *atname);// ������ �������� ����
int     xmlGetAttributesCount(xnode node);              // ���������� ��������� �������
xstring xmlGetAttributeName(xnode node, int index);     // ��� ��������
xstring xmlGetAttributeValue(xnode node, int index);    // �������� ��������
xstring xmlGetText(xnode node);                         // ����� ����� ������ � ������������ ������
xstring xmlGetCDataText(xnode node);                    // ������ CDATA � �������

int   xmlSave(xnode node, const char *filename);

xnode xmlCreateNode(xnode parentnode, const char* nodename);
xnode xmlCreateRootNode(const char* nodename); 
void  xmlDeleteNode(xnode node);
int   xmlSetAttribute(xnode node, const char *atname, const char *value);
int   xmlSetText(xnode node, const char *text);
xnode xmlGetParent(xnode node);                         // �������� ����������� ������� �� �������

xnode xmlMakeNodeCopy(xnode node, xnode parent_node);   // ����������� ������

#ifdef __cplusplus
}
#endif // __cplusplus
#endif // __XML_H__
