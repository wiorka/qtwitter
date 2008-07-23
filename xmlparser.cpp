#include "xmlparser.h"
#include "mainwindow.h"

#include <QtDebug>

XmlParser::XmlParser() :
  QXmlDefaultHandler(),
  lastField( None ),
  entry(),
  important( false )
  {
  }

bool XmlParser::startDocument() {
  //qDebug() << "Start of document";
  return true;
}

bool XmlParser::endDocument() {
  //qDebug() << "End of document";
  return true;
}

bool XmlParser::startElement( const QString & /* namespaceURI */, const QString & /* localName */, const QString &qName, const QXmlAttributes &atts ) {
  //qDebug() << "Start of element" << qName;
  
  ( (lastField = checkFieldType( qName )) != None ) ? important = true : important = false;
  //if (!entry && important) {
  //  entry = new Entry();
  //}
  
  for( int i = 0; i<atts.length(); ++i ) {
    //qDebug() << " " << atts.qName(i) << "=" << atts.value(i);
  }                                                                
  return true;
}

bool XmlParser::endElement( const QString & /* namespaceURI */, const QString & /* localName */, const QString &/*qName*/ ) {
  //qDebug() << "End of element" << qName;
  return true;
}

bool XmlParser::characters( const QString &ch ) {
  if ( important ) {
    if ( lastField == Name && !entry.name().compare( "" ) ) {
      entry.setName( ch );
      //qDebug() << "Setting name  with: " << ch;
    }
    if ( lastField == Text && !entry.text().compare( "" ) ) {
      entry.setText( ch );
      //qDebug() << "Setting text  with: " << ch;
    }
    if ( lastField == Image && !entry.image().compare( "" ) ) {
      entry.setImage( ch );
      //qDebug() << "Setting image with: " << ch;
    }  
    if ( entry.checkContents() ) {
      emit newEntry( entry );
      lastField = None;
      entry.setName( "" );
      entry.setText( "" );
      entry.setImage( "" );
    }
  }
  return true;
}

int XmlParser::checkFieldType(const QString &element ) {
  if ( !element.compare(USER_STATUS) )
    return Text;
  if ( !element.compare(USER_LOGIN) )
    return Name;
  if ( !element.compare(USER_PHOTO) )
    return Image;
  return None;
}
