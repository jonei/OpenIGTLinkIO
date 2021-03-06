/*==========================================================================

  Portions (c) Copyright 2008-2009 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $HeadURL: http://svn.slicer.org/Slicer4/trunk/Modules/OpenIGTLinkIF/vtkSlicerOpenIGTLinkIFLogic.cxx $
  Date:      $Date: 2011-06-12 14:55:20 -0400 (Sun, 12 Jun 2011) $
  Version:   $Revision: 16985 $

==========================================================================*/

// IGTLIO includes
#include "vtkIGTLIOLogic.h"
#include "vtkIGTLIOConnector.h"


#include <vtkObjectFactory.h>

// OpenIGTLinkIF Logic includes
#include <vtkNew.h>
#include <vtkCallbackCommand.h>
#include <vtkImageData.h>
#include <vtkTransform.h>

//---------------------------------------------------------------------------
vtkStandardNewMacro(vtkIGTLIOLogic);
//---------------------------------------------------------------------------
vtkIGTLIOLogic::vtkIGTLIOLogic()
{
}

//---------------------------------------------------------------------------
vtkIGTLIOLogic::~vtkIGTLIOLogic()
{
}

//---------------------------------------------------------------------------
void vtkIGTLIOLogic::PrintSelf(ostream& os, vtkIndent indent)
{
  os << indent << "vtkIGTLIOLogic:             " << this->GetClassName() << "\n";
}

//---------------------------------------------------------------------------
vtkIGTLIOConnectorPointer vtkIGTLIOLogic::CreateConnector()
{
  vtkIGTLIOConnectorPointer connector = vtkIGTLIOConnectorPointer::New();
  connector->SetUID(this->CreateUniqueConnectorID());
  std::stringstream ss;
  ss << "IGTLConnector_" << connector->GetUID();
  connector->SetName(ss.str());
  Connectors.push_back(connector);

//  typedef vtkSmartPointer<vtkCallbackCommand> vtkCallbackCommandPointer;
//  vtkCallbackCommandPointer connectorCallback = vtkCallbackCommandPointer::New();
//  connectorCallback->SetCallback ( vtkIGTLIOLogic::OnConnectorEvent );
//  connector->AddObserver (vtkIGTLIOConnector::DeviceModifiedEvent, connectorCallback );

  this->InvokeEvent(ConnectionAddedEvent, connector.GetPointer());
  return connector;
}

//void vtkIGTLIOLogic::OnConnectorEvent(vtkObject*, unsigned long eid, void* clientdata, void *calldata)
//{

//}

//---------------------------------------------------------------------------
int vtkIGTLIOLogic::CreateUniqueConnectorID() const
{
  int retval=0;
  for (int i=0; i<Connectors.size(); ++i)
    {
      retval = std::max(retval, Connectors[i]->GetUID()+1);
    }
  return retval;
}

//---------------------------------------------------------------------------
int vtkIGTLIOLogic::RemoveConnector(int index)
{
  std::vector<vtkIGTLIOConnectorPointer>::iterator toRemove = Connectors.begin()+index;
  this->InvokeEvent(ConnectionAboutToBeRemovedEvent, toRemove->GetPointer());
  Connectors.erase(toRemove);
  return 0;
}

//---------------------------------------------------------------------------
int vtkIGTLIOLogic::GetNumberOfConnectors() const
{
  return Connectors.size();
}

//---------------------------------------------------------------------------
vtkIGTLIOConnectorPointer vtkIGTLIOLogic::GetConnector(int index)
{
  return Connectors[index];
}

//---------------------------------------------------------------------------
void vtkIGTLIOLogic::PeriodicProcess()
{
  for (unsigned i=0; i<Connectors.size(); ++i)
    {
      Connectors[i]->PeriodicProcess();
    }
}


