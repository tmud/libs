#define IN_LIBXML
#include "../src/libxml.h"
#include "xml.h"
#include <libxml/parser.h>
//#include <libxml/xmlsave.h>
#include <libxml/xpath.h>
#include <encoding.h>
#include <vector>
typedef std::vector<xmlNodePtr> xmlNodeList;

xmlDocPtr  rootdoc;
xmlNodePtr rootnode;
//---------------------------------------------------------------------------------
void FormatTree(xmlNodePtr root)
{
    char symbols[4] = { 0x20, 0xd, 0xa, 0 };
    std::vector<xmlNodePtr> vNodesStack;
    while (root)
    {
        if (root->children)
            vNodesStack.push_back(root->children);

        xmlNodePtr tofree = NULL;
        if (root->type == XML_TEXT_NODE && root->content)
        {
            // обработка содержимого узла
            // удаляем все узлы, которые представляют собой только пробелы и переводы строк
            char *ptr = (char*)root->content;
            if (strspn(ptr, symbols) == strlen(ptr))
            {
                tofree = root;
            }
        }

        root = root->next;

        if (tofree)
        {
            xmlUnlinkNode(tofree);
            xmlFreeNode(tofree);
        }

        if (!root)
        {
            int last = vNodesStack.size() - 1;
            if (last >= 0) { root = vNodesStack[last]; vNodesStack.pop_back(); }
        }
    }
}
//---------------------------------------------------------------------------------
std::string error;
xstring xmlGetLoadError() {
    std::string e(error);
    if (!e.empty())
    {
        int last = e.length()-1;
        while (last >= 0 && (e.at(last)=='\n' || e.at(last)=='\r')) {
            last--;
        }
        error = e.substr(0, last);
    }
    return error.c_str();
}

void error_handler(void *ctx, const char *msg, ...) {
   const int TMP_BUF_SIZE = 1024;
   char string[TMP_BUF_SIZE];
   va_list arg_ptr;
   va_start(arg_ptr, msg);
   vsnprintf(string, TMP_BUF_SIZE, msg, arg_ptr);
   va_end(arg_ptr);
   error.append(string);
}

void clearLastError()
{
    error.clear();
}

xnode xmlLoad(const char *buffer, int size)
{
    clearLastError();
    if (!buffer || size <= 0)
        return NULL;

    xmlDocPtr doc = xmlReadMemory(buffer, size, NULL, "UTF-8", XML_PARSE_NOWARNING);
    if (!doc) return NULL;

    xmlNodePtr newnode = xmlCopyNode(xmlDocGetRootElement(doc), 1);
    xmlFreeDoc(doc);
    FormatTree(newnode);

    if (!xmlAddChild(rootnode, newnode))
    {
        xmlFreeNode(newnode); newnode = NULL;
    }
    return newnode;
}

xnode xmlLoadToNode(xnode node, const char *buffer, int size)
{
    clearLastError();
    if (!buffer || size <= 0 || !node)
        return NULL;

    xmlDocPtr doc = xmlReadMemory(buffer, size, NULL, "UTF-8", XML_PARSE_NOERROR | XML_PARSE_NOWARNING);
    if (!doc) return NULL;

    xmlNodePtr newnode = xmlCopyNode(xmlDocGetRootElement(doc), 1);
    xmlFreeDoc(doc);
    FormatTree(newnode);

    if (!xmlAddChild((xmlNodePtr)node, newnode))
    {
        xmlFreeNode(newnode); newnode = NULL;
    }
    return newnode;
}

/*xnode xmlLoad(const char *filename)
{
    clearLastError();
    if (!filename)
        return NULL;

    xmlDocPtr doc = xmlReadFile(filename, "UTF-8", XML_PARSE_NOWARNING);
    if (!doc) return NULL;

    xmlNodePtr newnode = xmlCopyNode(xmlDocGetRootElement(doc), 1);
    xmlFreeDoc(doc);
    FormatTree(newnode);

    if (!xmlAddChild(rootnode, newnode))
    {
        xmlFreeNode(newnode); newnode = NULL;
    }
    return newnode;
}

xnode xmlLoadToNode(xnode node, const char *filename)
{
    if (!filename || !node) 
        return NULL;

    xmlDocPtr doc = xmlReadFile(filename, "UTF-8", XML_PARSE_NOERROR | XML_PARSE_NOWARNING);
    if (!doc) return NULL;

    xmlNodePtr newnode = xmlCopyNode(xmlDocGetRootElement(doc), 1);
    xmlFreeDoc(doc);
    FormatTree(newnode);

    if (!xmlAddChild((xmlNodePtr)node, newnode))
    {
        xmlFreeNode(newnode); newnode = NULL;
    }
    return newnode;
}*/

xlist xmlRequest(xnode node, const char *request)
{
    if (!request || !node) 
        return NULL;

    xmlNodePtr reqnode = (xmlNodePtr)node;
    xmlXPathContextPtr xpathCtx = xmlXPathNewContext(rootdoc);
    if (!xpathCtx) return NULL;
    xpathCtx->node = reqnode;

    xmlChar *xcRequest = xmlCharStrdup(request);
    xmlXPathObjectPtr xpathObj = xmlXPathEval(xcRequest, xpathCtx);

    if (!xpathObj)
    {
        xmlXPathFreeContext(xpathCtx);
        return NULL;
    }

    if (xpathObj->type != XPATH_NODESET)
    {
        xmlXPathFreeObject(xpathObj);
        xmlXPathFreeContext(xpathCtx);
        return NULL;
    }

    xmlNodeList* newlist = NULL;
    xmlNodeSetPtr nodeset = xpathObj->nodesetval;
    if (nodeset && nodeset->nodeNr > 0)
    {
        int nodes = nodeset->nodeNr;
        newlist = new xmlNodeList;
        newlist->resize(nodes);
        for (int i = 0; i < nodes; i++)
           newlist->at(i) = xpathObj->nodesetval->nodeTab[i];
    }
    xmlXPathFreeObject(xpathObj);
    xmlXPathFreeContext(xpathCtx);
    free(xcRequest);
    return newlist;
}

void xmlDeleteAllData()
{
    xmlNodePtr temp = rootnode->children;
    while (temp != NULL)
    {
        xmlNodePtr ptr = temp;
        temp = temp->next;
        xmlUnlinkNode(ptr);
        xmlFreeNode(ptr);
    }
}

xstring xmlGetName(xnode node)
{
    if (!node) return NULL;
    xmlNodePtr _node = (xmlNodePtr)node;
    return (xstring) _node->name;
}

xstring xmlGetAttribute(xnode node, const char *atname)
{
    if (!atname || !node) return NULL;
    xmlChar *attribute = xmlCharStrdup(atname);
    xmlAttr *tmp = NULL;
    xmlNodePtr _node = (xmlNodePtr)node;
    for (xmlAttr* attr = _node->properties; attr; attr = attr->next)
    {
        if (!xmlStrcmp(attribute, attr->name))
        {
            tmp = attr; break;
        }
    }
    free(attribute);
    return (tmp) ? (xstring) tmp->children->content : NULL;
}

int xmlGetAttributesCount(xnode node)
{
    if (!node)
        return 0;
    int count = 0;
    xmlNodePtr _node = (xmlNodePtr)node;
    for (xmlAttr* attr = _node->properties; attr; attr = attr->next)
        count++;
    return count;
}

xstring xmlGetAttributeName(xnode node, int index)
{
    if (!node)
        return NULL;
    int count = 0;
    xmlNodePtr _node = (xmlNodePtr)node;
    for (xmlAttr* attr = _node->properties; attr; attr = attr->next)
    {
        if (index == count) { return (xstring) attr->name; }
        count++;
    }
    return NULL;
}

xstring xmlGetAttributeValue(xnode node, int index)
{
    if (!node)
        return NULL;
    int count = 0;
    xmlNodePtr _node = (xmlNodePtr)node;
    for (xmlAttr* attr = _node->properties; attr; attr = attr->next)
    {
        if (index == count) { return (xstring) attr->children->content; }
        count++;
    }
    return NULL;
}

xstring xmlGetText(xnode node)
{
    xmlNodePtr _node = (xmlNodePtr)node;
    if (!_node || !_node->children) return NULL;
    return (xstring) _node->children->content;
}

xstring xmlGetCDataText(xnode node)
{
    if (!node) return NULL;
    xmlNodePtr _node = (xmlNodePtr)node;
    _node = _node->children;
    while (_node && _node->type != XML_CDATA_SECTION_NODE)
        _node = _node->next;
    return (_node) ? (xstring) _node->content : NULL;
}

int xmlGetListSize(xlist nodelist)
{
    if (!nodelist) 
        return 0;
    xmlNodeList* list = (xmlNodeList*)nodelist;
    return list->size();
}

xnode xmlGetListNode(xlist nodelist, int index)
{
    if (!nodelist) 
        return NULL;
    xmlNodeList* list = (xmlNodeList*)nodelist;
    int size = list->size();
    return (index >=0 && index < size) ? list->at(index) : NULL;
}

void xmlFreeList(xlist nodelist)
{
    if (!nodelist) return;
    xmlNodeList* list = (xmlNodeList*)nodelist;
    delete list;
}

xbuffer xmlSave(xnode node)
{
    if (!node)
        return NULL;

    int res = 0;
    xmlBufferPtr buff = xmlBufferCreate();
    xmlOutputBufferPtr output_buffer = xmlOutputBufferCreateBuffer(buff, NULL);
    xmlNodePtr _node = (xmlNodePtr)node;
    xmlNodePtr oldrootnode = xmlDocSetRootElement(rootdoc, _node);
    int output_stream_len = xmlSaveFormatFileTo(output_buffer, rootdoc, "UTF-8", 1);
    xmlDocSetRootElement(rootdoc, oldrootnode);
    char* output = NULL;
    if (output_stream_len != -1)
    {
        output = new char[output_stream_len+1];
        memcpy(output, buff->content, output_stream_len);
        output[output_stream_len] = 0;
    }
    if (buff)
        xmlBufferFree(buff);
    return output;
}

void xmlFreeBuffer(xbuffer buffer)
{
    if (buffer) {
        delete []buffer;
    }
}

xnode xmlCreateRootNode(const char* nodename)
{
    return xmlCreateNode(0, nodename);
}

xnode xmlCreateNode(xnode parentnode, const char* nodename)
{
    if (!nodename) 
        return NULL;

    xmlNodePtr ptr = rootnode;
    if (parentnode)
        ptr = (xmlNodePtr)parentnode;
    if (!ptr) return NULL;

    xmlChar *nname = xmlCharStrdup(nodename);
    xmlNodePtr nptr = xmlNewChild(ptr, NULL, nname, NULL);

    free(nname);
    return nptr;
}

void xmlDeleteNode(xnode node)
{
    if (!node) return;
    xmlNodePtr _node = (xmlNodePtr)node;
    xmlUnlinkNode(_node);
    xmlFreeNode(_node);
}

int xmlSetAttribute(xnode node, const char *atname, const char *value)
{
    if (!atname || !value || !node) return 0;

    xmlChar *an = xmlCharStrdup(atname);
    xmlChar *v = xmlCharStrdup(value);

    xmlNodePtr _node = (xmlNodePtr)node;
    xmlAttr *new_attr = xmlNewProp(_node, an, v);

    free(an);
    free(v);

    return (new_attr) ? 1 : 0;
}

int xmlSetText(xnode node, const char *text)
{
    if (!node) return 0;
    xmlChar *an = xmlCharStrdup((text) ? text : "");
    xmlNodePtr _node = (xmlNodePtr)node;
    xmlNodeSetContent(_node, an);
    free(an);
    return 1;
}

xnode xmlGetParent(xnode node)
{
    if (!node) return NULL;
    xmlNodePtr _node = (xmlNodePtr)node;
    xmlNodePtr parent = _node->parent;
    if (rootnode == parent) return NULL;
    return parent;
}

xnode xmlMakeNodeCopy(xnode node, xnode parent_node)
{
    xmlNodePtr _node = (xmlNodePtr)node;
    xmlNodePtr new_node = xmlCopyNode(_node, 1);
    if (new_node)
    {
        xmlNodePtr _parentnode = (xmlNodePtr)node;
        if (!xmlAddChild(_parentnode, new_node))
        {
            xmlUnlinkNode(new_node);
            xmlFreeNode(new_node);
            new_node = NULL;
        }
    }
    return new_node;
}

void xmlInit()
{
    // Инициализируем парсер
    xmlInitParser();

    // Создаем пустой документ и корневой элемент - Root,
    // куда в дальнейшем будем загружать xml файлы, создавая общее xml-дерево	
    rootdoc = xmlNewDoc((xmlChar*)"1.0");
    rootnode = xmlNewDocNode(rootdoc, NULL, (xmlChar*)"root", NULL);
    xmlDocSetRootElement(rootdoc, rootnode);
    xmlSetGenericErrorFunc(NULL, error_handler);
}

void xmlDestroy()
{
    // Освобождаем все ресурсы
    xmlSetGenericErrorFunc(NULL, NULL);
    xmlDeleteAllData();
    xmlFreeDoc(rootdoc);
    xmlCleanupParser();
}
