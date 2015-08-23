// Reminder: Modify typemap.dat to customize the header file generated by wsdl2h
/* auto\tmp.h
   Generated by wsdl2h 2.8.16 from ht_settings.xsd and c:\work\xp-dev\baseline\happy_trader\extern\tools\gsoap-2.8\gsoap\typemap.dat
   2015-08-22 12:44:45 GMT

   DO NOT INCLUDE THIS FILE DIRECTLY INTO YOUR PROJECT BUILDS
   USE THE soapcpp2-GENERATED SOURCE CODE FILES FOR YOUR PROJECT BUILDS

   gSOAP XML Web services tools.
   Copyright (C) 2000-2013 Robert van Engelen, Genivia Inc. All Rights Reserved.
   Part of this software is released under one of the following licenses:
   GPL or Genivia's license for commercial use.
*/

/** @page page_notes Usage Notes

NOTE:

 - Run soapcpp2 on auto\tmp.h to generate the SOAP/XML processing logic.
   Use soapcpp2 -I to specify paths for #import
   To build with STL, 'stlvector.h' is imported from 'import' dir in package.
   Use soapcpp2 -j to generate improved proxy and server classes.
 - Use wsdl2h -c and -s to generate pure C code or C++ code without STL.
 - Use 'typemap.dat' to control namespace bindings and type mappings.
   It is strongly recommended to customize the names of the namespace prefixes
   generated by wsdl2h. To do so, modify the prefix bindings in the Namespaces
   section below and add the modified lines to 'typemap.dat' to rerun wsdl2h.
 - Use Doxygen (www.doxygen.org) on this file to generate documentation.
 - Use wsdl2h -R to generate REST operations.
 - Use wsdl2h -nname to use name as the base namespace prefix instead of 'ns'.
 - Use wsdl2h -Nname for service prefix and produce multiple service bindings
 - Use wsdl2h -d to enable DOM support for xsd:anyType.
 - Use wsdl2h -g to auto-generate readers and writers for root elements.
 - Use wsdl2h -b to auto-generate bi-directional operations (duplex ops).
 - Struct/class members serialized as XML attributes are annotated with a '@'.
 - Struct/class members that have a special role are annotated with a '$'.

WARNING:

   DO NOT INCLUDE THIS ANNOTATED FILE DIRECTLY IN YOUR PROJECT SOURCE CODE.
   USE THE FILES GENERATED BY soapcpp2 FOR YOUR PROJECT'S SOURCE CODE:
   THE soapStub.h FILE CONTAINS THIS CONTENT WITHOUT ANNOTATIONS.

LICENSE:

@verbatim
--------------------------------------------------------------------------------
gSOAP XML Web services tools
Copyright (C) 2000-2013, Robert van Engelen, Genivia Inc. All Rights Reserved.

This software is released under one of the following licenses:
GPL or Genivia's license for commercial use.
--------------------------------------------------------------------------------
GPL license.

This program is free software; you can redistribute it and/or modify it under
the terms of the GNU General Public License as published by the Free Software
Foundation; either version 2 of the License, or (at your option) any later
version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with
this program; if not, write to the Free Software Foundation, Inc., 59 Temple
Place, Suite 330, Boston, MA 02111-1307 USA

Author contact information:
engelen@genivia.com / engelen@acm.org

This program is released under the GPL with the additional exemption that
compiling, linking, and/or using OpenSSL is allowed.
--------------------------------------------------------------------------------
A commercial-use license is available from Genivia, Inc., contact@genivia.com
--------------------------------------------------------------------------------
@endverbatim

*/


//gsoapopt w

/******************************************************************************\
 *                                                                            *
 * Definitions                                                                *
 *   http://www.foo.com                                                       *
 *                                                                            *
\******************************************************************************/


/******************************************************************************\
 *                                                                            *
 * Import                                                                     *
 *                                                                            *
\******************************************************************************/


// STL vector containers (use option -s to remove STL dependency)
#import "stlvector.h"

/******************************************************************************\
 *                                                                            *
 * Schema Namespaces                                                          *
 *                                                                            *
\******************************************************************************/


/* NOTE:

It is strongly recommended to customize the names of the namespace prefixes
generated by wsdl2h. To do so, modify the prefix bindings below and add the
modified lines to typemap.dat to rerun wsdl2h:

ns1 = "http://www.foo.com"

*/

#define SOAP_NAMESPACE_OF_ns1	"http://www.foo.com"
//gsoap ns1   schema namespace:	http://www.foo.com
//gsoap ns1   schema elementForm:	qualified
//gsoap ns1   schema attributeForm:	unqualified

/******************************************************************************\
 *                                                                            *
 * Built-in Schema Types and Top-Level Elements and Attributes                *
 *                                                                            *
\******************************************************************************/


/// Primitive built-in type "xs:integer"
typedef std::string xsd__integer;

/******************************************************************************\
 *                                                                            *
 * Forward Declarations                                                       *
 *                                                                            *
\******************************************************************************/



//  Forward declaration of class _ns1__settings.
class _ns1__settings;

/******************************************************************************\
 *                                                                            *
 * Schema Types and Top-Level Elements and Attributes                         *
 *   http://www.foo.com                                                       *
 *                                                                            *
\******************************************************************************/


/******************************************************************************\
 *                                                                            *
 * Schema Complex Types and Top-Level Elements                                *
 *   http://www.foo.com                                                       *
 *                                                                            *
\******************************************************************************/



/// Top-level root element "http://www.foo.com":settings

/// "http://www.foo.com":settings is a complexType.
/// class _ns1__settings operations:
/// - soap_new__ns1__settings(soap*) allocate
/// - soap_new__ns1__settings(soap*, int num) allocate array
/// - soap_new_req__ns1__settings(soap*, ...) allocate, set required members
/// - soap_new_set__ns1__settings(soap*, ...) allocate, set all public members
/// - int soap_read__ns1__settings(soap*, _ns1__settings*) deserialize from a stream
/// - int soap_write__ns1__settings(soap, _ns1__settings*) serialize to a stream
class _ns1__settings
{ public:
/// Element mt4loglevel of type xs:integer.
    xsd__integer                         mt4loglevel                    1;	///< Required element.
/// Element heartbeatjavahtsec of type xs:integer.
    xsd__integer                         heartbeatjavahtsec             1;	///< Required element.
/// Element mt4datareadintrvalsec of type xs:integer.
    xsd__integer                         mt4datareadintrvalsec          1;	///< Required element.
/// Element zombiemt4killer of type xs:integer.
    xsd__integer                         zombiemt4killer                1;	///< Required element.
/// class _ns1__settings_mtinstances operations:
/// - soap_new__ns1__settings_mtinstances(soap*) allocate
/// - soap_new__ns1__settings_mtinstances(soap*, int num) allocate array
/// - soap_new_req__ns1__settings_mtinstances(soap*, ...) allocate, set required members
/// - soap_new_set__ns1__settings_mtinstances(soap*, ...) allocate, set all public members
/// - int soap_read__ns1__settings_mtinstances(soap*, _ns1__settings_mtinstances*) deserialize from a stream
/// - int soap_write__ns1__settings_mtinstances(soap, _ns1__settings_mtinstances*) serialize to a stream
    class _ns1__settings_mtinstances
    {
/// Vector of instance with length 1..unbounded
    std::vector<
/// class _ns1__settings_mtinstances_instance operations:
/// - soap_new__ns1__settings_mtinstances_instance(soap*) allocate
/// - soap_new__ns1__settings_mtinstances_instance(soap*, int num) allocate array
/// - soap_new_req__ns1__settings_mtinstances_instance(soap*, ...) allocate, set required members
/// - soap_new_set__ns1__settings_mtinstances_instance(soap*, ...) allocate, set all public members
/// - int soap_read__ns1__settings_mtinstances_instance(soap*, _ns1__settings_mtinstances_instance*) deserialize from a stream
/// - int soap_write__ns1__settings_mtinstances_instance(soap, _ns1__settings_mtinstances_instance*) serialize to a stream
    class _ns1__settings_mtinstances_instance
    {
/// Element path of type xs:string.
    std::string                          path                           1;	///< Required element.
/// Element name of type xs:string.
    std::string                          name                           1;	///< Required element.
/// Element loadbalanced of type xs:string.
    std::string                          loadbalanced                   1;	///< Required element.
/// Element autorecover of type xs:string.
    std::string*                         autorecover                    0;	///< Optional element.
/// Element autoinstall of type xs:string.
    std::string*                         autoinstall                    0;	///< Optional element.
/// Element autoload of type xs:string.
    std::string*                         autoload                       0;	///< Optional element.
/// Element autoqueuerestore of type xs:string.
    std::string*                         autoqueuerestore               0;	///< Optional element.
/// Element companyid of type xs:string.
    std::string                          companyid                      1;	///< Required element.
/// Element mt_distrib of type xs:string.
    std::string                          mt_USCOREdistrib               1;	///< Required element.
    }>                                   instance                       1;
    }                                    mtinstances                    1;	///< Required element.
/// class _ns1__settings_generalparams operations:
/// - soap_new__ns1__settings_generalparams(soap*) allocate
/// - soap_new__ns1__settings_generalparams(soap*, int num) allocate array
/// - soap_new_req__ns1__settings_generalparams(soap*, ...) allocate, set required members
/// - soap_new_set__ns1__settings_generalparams(soap*, ...) allocate, set all public members
/// - int soap_read__ns1__settings_generalparams(soap*, _ns1__settings_generalparams*) deserialize from a stream
/// - int soap_write__ns1__settings_generalparams(soap, _ns1__settings_generalparams*) serialize to a stream
    class _ns1__settings_generalparams
    {
/// class _ns1__settings_generalparams_htserver operations:
/// - soap_new__ns1__settings_generalparams_htserver(soap*) allocate
/// - soap_new__ns1__settings_generalparams_htserver(soap*, int num) allocate array
/// - soap_new_req__ns1__settings_generalparams_htserver(soap*, ...) allocate, set required members
/// - soap_new_set__ns1__settings_generalparams_htserver(soap*, ...) allocate, set all public members
/// - int soap_read__ns1__settings_generalparams_htserver(soap*, _ns1__settings_generalparams_htserver*) deserialize from a stream
/// - int soap_write__ns1__settings_generalparams_htserver(soap, _ns1__settings_generalparams_htserver*) serialize to a stream
    class _ns1__settings_generalparams_htserver
    {
/// Element debugconsole of type xs:string.
    std::string*                         debugconsole                   0;	///< Optional element.
/// Element port of type xs:integer.
    xsd__integer                         port                           1;	///< Required element.
/// Element internalloglevel of type xs:integer.
    xsd__integer                         internalloglevel               1;	///< Required element.
/// Element eventlevel of type xs:integer.
    xsd__integer                         eventlevel                     1;	///< Required element.
    }                                    htserver                       1;	///< Required element.
/// class _ns1__settings_generalparams_eventpluginparams operations:
/// - soap_new__ns1__settings_generalparams_eventpluginparams(soap*) allocate
/// - soap_new__ns1__settings_generalparams_eventpluginparams(soap*, int num) allocate array
/// - soap_new_req__ns1__settings_generalparams_eventpluginparams(soap*, ...) allocate, set required members
/// - soap_new_set__ns1__settings_generalparams_eventpluginparams(soap*, ...) allocate, set all public members
/// - int soap_read__ns1__settings_generalparams_eventpluginparams(soap*, _ns1__settings_generalparams_eventpluginparams*) deserialize from a stream
/// - int soap_write__ns1__settings_generalparams_eventpluginparams(soap, _ns1__settings_generalparams_eventpluginparams*) serialize to a stream
    class _ns1__settings_generalparams_eventpluginparams
    {
/// Vector of entry with length 1..unbounded
    std::vector<
/// class _ns1__settings_generalparams_eventpluginparams_entry operations:
/// - soap_new__ns1__settings_generalparams_eventpluginparams_entry(soap*) allocate
/// - soap_new__ns1__settings_generalparams_eventpluginparams_entry(soap*, int num) allocate array
/// - soap_new_req__ns1__settings_generalparams_eventpluginparams_entry(soap*, ...) allocate, set required members
/// - soap_new_set__ns1__settings_generalparams_eventpluginparams_entry(soap*, ...) allocate, set all public members
/// - int soap_read__ns1__settings_generalparams_eventpluginparams_entry(soap*, _ns1__settings_generalparams_eventpluginparams_entry*) deserialize from a stream
/// - int soap_write__ns1__settings_generalparams_eventpluginparams_entry(soap, _ns1__settings_generalparams_eventpluginparams_entry*) serialize to a stream
    class _ns1__settings_generalparams_eventpluginparams_entry
    {
/// Element name of type xs:string.
    std::string                          name                           1;	///< Required element.
/// Element value of type xs:string.
    std::string                          value                          1;	///< Required element.
    }>                                   entry                          1;
    }                                    eventpluginparams              1;	///< Required element.
    }                                    generalparams                  1;	///< Required element.
/// A handle to the soap struct that manages this instance (automatically set)
    struct soap                         *soap                          ;
};

/******************************************************************************\
 *                                                                            *
 * Additional Top-Level Elements                                              *
 *   http://www.foo.com                                                       *
 *                                                                            *
\******************************************************************************/


/******************************************************************************\
 *                                                                            *
 * Additional Top-Level Attributes                                            *
 *   http://www.foo.com                                                       *
 *                                                                            *
\******************************************************************************/


/******************************************************************************\
 *                                                                            *
 * XML Data Binding                                                           *
 *                                                                            *
\******************************************************************************/


/**

@page page_XMLDataBinding XML Data Binding

SOAP/XML services use data bindings contractually bound by WSDL and auto-
generated by wsdl2h and soapcpp2 (see Service Bindings). Plain data bindings
are adopted from XML schemas as part of the WSDL types section or when running
wsdl2h on a set of schemas to produce non-SOAP-based XML data bindings.

The following readers and writers are C/C++ data type (de)serializers auto-
generated by wsdl2h and soapcpp2. Run soapcpp2 on this file to generate the
(de)serialization code, which is stored in soapC.c[pp]. Include "soapH.h" in
your code to import these data type and function declarations. Only use the
soapcpp2-generated files in your project build. Do not include the wsdl2h-
generated .h file in your code.

Data can be read in XML and deserialized from:
  - a file descriptor, using soap->recvfd = fd
  - a socket, using soap->socket = ...
  - a C++ stream, using soap->is = ...
  - a buffer, using the soap->frecv() callback

Data can be serialized in XML and written to:
  - a file descriptor, using soap->sendfd = fd
  - a socket, using soap->socket = ...
  - a C++ stream, using soap->os = ...
  - a buffer, using the soap->fsend() callback

The following options are available for (de)serialization control:
  - soap->encodingStyle = NULL; to remove SOAP 1.1/1.2 encodingStyle
  - soap_mode(soap, SOAP_XML_TREE); XML without id-ref (no cycles!)
  - soap_mode(soap, SOAP_XML_GRAPH); XML with id-ref (including cycles)
  - soap_set_namespaces(soap, struct Namespace *nsmap); to set xmlns bindings


@section ns1 Top-level root elements of schema "http://www.foo.com"

  - <ns1:settings> @ref _ns1__settings
    @code
    // Reader (returns SOAP_OK on success):
    soap_read__ns1__settings(struct soap*, _ns1__settings*);
    // Writer (returns SOAP_OK on success):
    soap_write__ns1__settings(struct soap*, _ns1__settings*);
    @endcode

*/

/* End of auto\tmp.h */