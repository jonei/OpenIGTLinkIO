/*==========================================================================

  Portions (c) Copyright 2008-2009 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $HeadURL: http://svn.slicer.org/Slicer3/trunk/Modules/OpenIGTLinkIF/vtkIGTLDevice.h $
  Date:      $Date: 2010-11-23 00:58:13 -0500 (Tue, 23 Nov 2010) $
  Version:   $Revision: 15552 $

==========================================================================*/

#ifndef __vtkIGTLIODevice_h
#define __vtkIGTLIODevice_h

// OpenIGTLink includes
#include <igtlMessageBase.h>

// VTK includes
#include <vtkObject.h>
#include <vtkSmartPointer.h>

// STD includes
#include <vector>
#include <string>
#include <set>

// igtlio includes
#include "igtlioDevicesExport.h"
#include "igtlBaseConverter.h"

class vtkIGTLIODevice;

class vtkIGTLIODeviceCreator : public vtkObject
{
public:
 // Create an instance of the specific device, with the given device_id
 virtual vtkSmartPointer<vtkIGTLIODevice> Create(std::string device_name) = 0;
 // Return the device_type this factory creates devices for.
 virtual std::string GetDeviceType() const = 0;

  vtkAbstractTypeMacro(vtkIGTLIODeviceCreator,vtkObject);

};
//---------------------------------------------------------------------------

/// A vtkIGTLIODevice represents one device connected over OpenIGTLink.
///
/// The Device has a specific type, e.g. Image, Transform..., and
/// is capable of receiving and emitting igtl messages for that type.
///
/// One Device is bound to a device name, corresponding to an igtl device name.
///
class OPENIGTLINKIO_DEVICES_EXPORT vtkIGTLIODevice : public vtkObject
{
public:
  enum MESSAGE_DIRECTION {
    MESSAGE_DIRECTION_IN,
    MESSAGE_DIRECTION_OUT,
    NUM_MESSAGE_DIRECTION,
  };

 enum MESSAGE_PREFIX {
   MESSAGE_PREFIX_NOT_DEFINED,
   MESSAGE_PREFIX_GET,
   MESSAGE_PREFIX_START,
   MESSAGE_PREFIX_STOP,
   NUM_MESSAGE_PREFIX,
 };
 enum QUERY_STATUS {
   QUERY_STATUS_NONE,
   QUERY_STATUS_WAITING,
   QUERY_STATUS_SUCCESS,
   QUERY_STATUS_EXPIRED,
   QUERY_STATUS_ERROR,
   NUM_QUERY_STATUS,
 };
 enum {
   ConnectedEvent        = 118944,
   DisconnectedEvent     = 118945,
   ActivatedEvent        = 118946,
   DeactivatedEvent      = 118947,
   ReceiveEvent          = 118948,
   NewDeviceEvent        = 118949,
   DeviceModifiedEvent   = 118950,
   RemovedDeviceEvent    = 118951,
   ResponseEvent         = 118952,
 };


public:
 virtual std::string GetDeviceType() const = 0;
// virtual std::string GetDeviceName() const = 0;

 virtual std::string GetDeviceName() const;
  /// Set device name.
  /// Caution: Changing the device name of a device registered in
  /// a vtkIGTLIOConnector will cause undefined behaviour.
  virtual void SetDeviceName(std::string name);

 vtkSetMacro( PushOnConnect, bool );
 vtkGetMacro( PushOnConnect, bool );
 vtkSetMacro( MessageDirection, MESSAGE_DIRECTION );
 vtkGetMacro( MessageDirection, MESSAGE_DIRECTION );
 vtkSetMacro( QueryTimeOut, double );
 vtkGetMacro( QueryTimeOut, double );

 virtual double GetTimestamp() const;
 virtual void SetTimestamp(double val);

 void PrintSelf(ostream& os, vtkIndent indent);

 bool MessageDirectionIsOut() const { return MessageDirection==MESSAGE_DIRECTION_OUT; }
 bool MessageDirectionIsIn() const { return MessageDirection==MESSAGE_DIRECTION_IN; }

 // Read the content of buffer and update the device accordingly.
 // This assumes the incoming message has a device_type corresponding to this device
 virtual int ReceiveIGTLMessage(igtl::MessageBase::Pointer buffer, bool checkCRC) = 0;

 // Write the Device content into buffer.
 // The returned pointer might be allocated internally once
 // and reused between successive calls.
 virtual igtl::MessageBase::Pointer GetIGTLMessage() = 0;
 // As GetIGTLMessage(), except that the generated message is for the given prefix.
 // TODO: merge with above
 virtual igtl::MessageBase::Pointer GetIGTLMessage(MESSAGE_PREFIX prefix) { return igtl::MessageBase::Pointer(); }

 // Return supported messages, in the set GET, STT, STP, RTS,
 // The default message equals GetDeviceType() and is always supported,
 // prefixed messages are <prefix>_<device_type>.
 virtual std::set<MESSAGE_PREFIX> GetSupportedMessagePrefixes() const { return std::set<MESSAGE_PREFIX>(); }

 // TODO: add old features from Connector:
 //       - lock (means dont accept incoming messages),
 //       - gettimestamp (of last incoming message)

 // TODO: notify listeners when:
 // created, deleted, modified
 // This can be done from the Connector or Logic instead, this would enable all 3 signals at the same level.

 /// Query handling:
 /// Each device has a list of queries (GET_, STT_, STP_) that has been sent
 /// and are awaiting reply.
 //
 /// Device::GetMessage() pushes a query,
 /// Device::ReceiveMessage() processes the reply, and emits events for the receive
 ///   - statechange: waiting, success, expired,...
 //
 //
 /// One query (GET_, STT_ or STP_-message that requires an answer)
 struct QueryType
 {
   std::string query_message;
   std::string response_message;
   double query_timestamp;
   QUERY_STATUS status;
 };

  /// Get all current queries
  std::vector<QueryType> GetQueries() const;
  /// check for waiting queries that have waited beoynd the timeout for an answer, mark them as expired.
  int CheckQueryExpiration();
  /// remove all queries that are answered or expired.
  int PruneCompletedQueries();
  int CancelQuery(int index);

 public:

  vtkAbstractTypeMacro(vtkIGTLIODevice,vtkObject);

  igtl::BaseConverter::HeaderData HeaderData;

private:
 std::vector<QueryType> Queries;
 MESSAGE_DIRECTION MessageDirection;
 bool PushOnConnect;
 double QueryTimeOut;


 protected:
  vtkIGTLIODevice();
  ~vtkIGTLIODevice();
};


#endif //__vtkIGTLIODevice_h
