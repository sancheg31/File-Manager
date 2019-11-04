#include "Interfaces/IDocument.h"

IDocument::IDocument(QWidget* wgt): QTextEdit(wgt) {}

IDocument::~IDocument() { }
