/**********************************************************************
 *  Copyright (c) 2008-2013, Alliance for Sustainable Energy.
 *  All rights reserved.
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2.1 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 **********************************************************************/

#include <energyplus/ForwardTranslator.hpp>

#include <model/Model.hpp>
#include <model/ShadingSurface.hpp>
#include <model/ShadingSurface_Impl.hpp>
#include <model/ShadingSurfaceGroup.hpp>
#include <model/ShadingSurfaceGroup_Impl.hpp>
#include <model/Space.hpp>
#include <model/Space_Impl.hpp>
#include <model/Surface.hpp>
#include <model/Surface_Impl.hpp>
#include <model/Building.hpp>
#include <model/Building_Impl.hpp>
#include <model/Schedule.hpp>
#include <model/Schedule_Impl.hpp>

#include <utilities/idf/IdfExtensibleGroup.hpp>
#include <utilities/geometry/Transformation.hpp>
#include <utilities/geometry/Geometry.hpp>

#include <utilities/idd/Shading_Site_Detailed_FieldEnums.hxx>
#include <utilities/idd/Shading_Building_Detailed_FieldEnums.hxx>
#include <utilities/idd/Shading_Zone_Detailed_FieldEnums.hxx>

#include <utilities/idd/IddEnums.hxx>
#include <utilities/idd/IddFactory.hxx>

#include <utilities/core/Assert.hpp>

using namespace openstudio::model;

using namespace std;

namespace openstudio {

namespace energyplus {

boost::optional<IdfObject> ForwardTranslator::translateShadingSurface( model::ShadingSurface & modelObject )
{
  boost::optional<IdfObject> idfObject;

  boost::optional<Schedule> transmittanceSchedule = modelObject.transmittanceSchedule();
  Transformation transformation;
  Point3dVector points;

  boost::optional<ShadingSurfaceGroup> shadingSurfaceGroup = modelObject.shadingSurfaceGroup();
  if (shadingSurfaceGroup){

    transformation = shadingSurfaceGroup->transformation();
    points = transformation * modelObject.vertices();

    if (istringEqual("Space", shadingSurfaceGroup->shadingSurfaceType())){

      idfObject = IdfObject(openstudio::IddObjectType::Shading_Zone_Detailed);
      idfObject->setString(Shading_Zone_DetailedFields::Name, modelObject.name().get());

      boost::optional<Space> space = shadingSurfaceGroup->space();
      if (space){
        
        boost::optional<Surface> baseSurface;
        double minDistance = std::numeric_limits<double>::max();

        // at this point zone has one space and internal surfaces have already been combined
        BOOST_FOREACH(const Surface& surface, space->surfaces()){
          if (istringEqual(surface.outsideBoundaryCondition(), "Outdoors")){
            Point3dVector surfaceVertices = surface.vertices();
            BOOST_FOREACH(const Point3d& point, points){
              BOOST_FOREACH(const Point3d& surfaceVertex, surfaceVertices){
                double distance = getDistance(point, surfaceVertex);
                if (distance < minDistance){
                  baseSurface = surface;
                  minDistance = distance;
                }
              }
            }
          }
        }

        if (!baseSurface){
          LOG(Error, "Cannot find appropriate base surface for shading surface '" << modelObject.name().get() << 
                     "', the shading surface will not be translated");
          return boost::none;
        }

        idfObject->setString(Shading_Zone_DetailedFields::BaseSurfaceName, baseSurface->name().get());
      }

      if (transmittanceSchedule){
        idfObject->setString(Shading_Zone_DetailedFields::TransmittanceScheduleName, transmittanceSchedule->name().get());
      }

    }else if (istringEqual("Site", shadingSurfaceGroup->shadingSurfaceType())){

      idfObject = IdfObject(openstudio::IddObjectType::Shading_Site_Detailed);
      idfObject->setString(Shading_Site_DetailedFields::Name, modelObject.name().get());
      
      if (transmittanceSchedule){
        idfObject->setString(Shading_Site_DetailedFields::TransmittanceScheduleName, transmittanceSchedule->name().get());
      }

    }else{
      boost::optional<Building> building = modelObject.model().getUniqueModelObject<Building>();
      if (building){
        transformation = building->transformation().inverse()*transformation;
      }

      idfObject = IdfObject(openstudio::IddObjectType::Shading_Building_Detailed);
      idfObject->setString(Shading_Building_DetailedFields::Name, modelObject.name().get());
      
      if (transmittanceSchedule){
        idfObject->setString(Shading_Building_DetailedFields::TransmittanceScheduleName, transmittanceSchedule->name().get());
      }
    }

  }else{
    idfObject = IdfObject(openstudio::IddObjectType::Shading_Building_Detailed);
    idfObject->setString(Shading_Building_DetailedFields::Name, modelObject.name().get());
    
    if (transmittanceSchedule){
      idfObject->setString(Shading_Building_DetailedFields::TransmittanceScheduleName, transmittanceSchedule->name().get());
    }
  }

  m_idfObjects.push_back(*idfObject);

  idfObject->clearExtensibleGroups();

  BOOST_FOREACH(const Point3d& point, points){
    IdfExtensibleGroup group = idfObject->pushExtensibleGroup();
    OS_ASSERT(group.numFields() == 3);
    group.setDouble(0, point.x());
    group.setDouble(1, point.y());
    group.setDouble(2, point.z());
  }

  return idfObject;
}

} // energyplus

} // openstudio

