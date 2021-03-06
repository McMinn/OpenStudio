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

#include <gtest/gtest.h>

#include <model/test/ModelFixture.hpp>
#include <model/Model_Impl.hpp>
#include <model/Site.hpp>
#include <model/Site_Impl.hpp>
#include <model/Building.hpp>
#include <model/Building_Impl.hpp>
#include <model/Space.hpp>
#include <model/Space_Impl.hpp>
#include <model/ShadingSurface.hpp>
#include <model/ShadingSurface_Impl.hpp>
#include <model/ShadingSurfaceGroup.hpp>
#include <model/ShadingSurfaceGroup_Impl.hpp>

#include <boost/foreach.hpp>

using namespace openstudio::model;
using namespace openstudio;

TEST_F(ModelFixture, ShadingSurfaceGroup_Space_Hierarchy)
{
  Model model;

  // add a space
  Space space(model);

  // add a shading surface group
  ShadingSurfaceGroup group(model);
  EXPECT_EQ("Building", group.shadingSurfaceType());
  EXPECT_TRUE(group.setSpace(space));
  EXPECT_EQ("Space", group.shadingSurfaceType());
  ASSERT_TRUE(group.space());
  EXPECT_EQ(space.handle(), group.space()->handle());

  // make a new shading surface
  Point3dVector points;
  points.push_back(Point3d(0, 2, 0));
  points.push_back(Point3d(0, 0, 0));
  points.push_back(Point3d(1, 0, 0));
  ShadingSurface surface(points, model);
  EXPECT_TRUE(surface.setShadingSurfaceGroup(group));
  ASSERT_TRUE(surface.shadingSurfaceGroup());
  EXPECT_EQ(group.handle(), surface.shadingSurfaceGroup()->handle());

  // check children from group
  ASSERT_EQ(static_cast<unsigned>(1), group.children().size());
  EXPECT_EQ(surface.handle(), group.children()[0].handle());

  // now check children from space
  ASSERT_EQ(static_cast<unsigned>(1), space.children().size());
  EXPECT_EQ(group.handle(), space.children()[0].handle());

  // check parent from group
  ASSERT_TRUE(group.parent());
  EXPECT_EQ(space.handle(), group.parent()->handle());

  // check parent from surface
  ASSERT_TRUE(surface.parent());
  EXPECT_EQ(group.handle(), surface.parent()->handle());

  // clone the space
  ModelObject clone = space.clone(model);
  OptionalSpace spaceClone = clone.optionalCast<Space>();
  ASSERT_TRUE(spaceClone);
  ASSERT_EQ(static_cast<unsigned>(1), spaceClone->children().size());
  EXPECT_NE(group.handle(), spaceClone->children()[0].handle());
  OptionalShadingSurfaceGroup cloneChild = spaceClone->children()[0].optionalCast<ShadingSurfaceGroup>();
  ASSERT_TRUE(cloneChild);
  ASSERT_EQ(static_cast<unsigned>(1), cloneChild->children().size());
  EXPECT_NE(surface.handle(), cloneChild->children()[0].handle());
}

TEST_F(ModelFixture, ShadingSurfaceGroup_Building_Hierarchy)
{
  Model model;

  // make a building
  Building building = model.getUniqueModelObject<Building>();

  // add a shading surface group
  ShadingSurfaceGroup group(model);
  EXPECT_TRUE(group.setShadingSurfaceType("Building"));

  // make a new shading surface
  Point3dVector points;
  points.push_back(Point3d(0, 2, 0));
  points.push_back(Point3d(0, 0, 0));
  points.push_back(Point3d(1, 0, 0));
  ShadingSurface surface(points, model);
  EXPECT_TRUE(surface.setShadingSurfaceGroup(group));
  ASSERT_TRUE(surface.shadingSurfaceGroup());
  EXPECT_EQ(group.handle(), surface.shadingSurfaceGroup()->handle());

  // check children from group
  ASSERT_EQ(static_cast<unsigned>(1), group.children().size());
  EXPECT_EQ(surface.handle(), group.children()[0].handle());

  // now check children from building
  ASSERT_EQ(static_cast<unsigned>(1), building.children().size());
  EXPECT_EQ(group.handle(), building.children()[0].handle());

  // check parent from group
  ASSERT_TRUE(group.parent());
  EXPECT_EQ(building.handle(), group.parent()->handle());

  // check parent from surface
  ASSERT_TRUE(surface.parent());
  EXPECT_EQ(group.handle(), surface.parent()->handle());
}

TEST_F(ModelFixture, ShadingSurfaceGroup_Site_Hierarchy)
{
  Model model;

  // make a site
  Site site = model.getUniqueModelObject<Site>();

  // add a shading surface group
  ShadingSurfaceGroup group(model);
  EXPECT_TRUE(group.setShadingSurfaceType("Site"));

  // make a new shading surface
  Point3dVector points;
  points.push_back(Point3d(0, 2, 0));
  points.push_back(Point3d(0, 0, 0));
  points.push_back(Point3d(1, 0, 0));
  ShadingSurface surface(points, model);
  EXPECT_TRUE(surface.setShadingSurfaceGroup(group));
  ASSERT_TRUE(surface.shadingSurfaceGroup());
  EXPECT_EQ(group.handle(), surface.shadingSurfaceGroup()->handle());

  // check children from group
  ASSERT_EQ(static_cast<unsigned>(1), group.children().size());
  EXPECT_EQ(surface.handle(), group.children()[0].handle());

  // now check children from site
  ASSERT_EQ(static_cast<unsigned>(1), site.children().size());
  EXPECT_EQ(group.handle(), site.children()[0].handle());

  // check parent from group
  ASSERT_TRUE(group.parent());
  EXPECT_EQ(site.handle(), group.parent()->handle());

  // check parent from surface
  ASSERT_TRUE(surface.parent());
  EXPECT_EQ(group.handle(), surface.parent()->handle());
}

TEST_F(ModelFixture, ShadingSurfaceGroup_SetParent)
{
  Model model;

  Site site = model.getUniqueModelObject<Site>();
  Building building = model.getUniqueModelObject<Building>();
  Space space(model);

  ShadingSurfaceGroup group(model);
  EXPECT_EQ("Building", group.shadingSurfaceType());
  ASSERT_TRUE(group.parent());
  EXPECT_EQ(building.handle(), group.parent()->handle());
  EXPECT_FALSE(group.space());

  EXPECT_TRUE(group.setParent(site));
  EXPECT_EQ("Site", group.shadingSurfaceType());
  ASSERT_TRUE(group.parent());
  EXPECT_EQ(site.handle(), group.parent()->handle());
  EXPECT_FALSE(group.space());

  EXPECT_TRUE(group.setParent(space));
  EXPECT_EQ("Space", group.shadingSurfaceType());
  ASSERT_TRUE(group.parent());
  EXPECT_EQ(space.handle(), group.parent()->handle());
  ASSERT_TRUE(group.space());
  EXPECT_EQ(space.handle(), group.space()->handle());

  EXPECT_TRUE(group.setParent(building));
  EXPECT_EQ("Building", group.shadingSurfaceType());
  ASSERT_TRUE(group.parent());
  EXPECT_EQ(building.handle(), group.parent()->handle());
  EXPECT_FALSE(group.space());
}