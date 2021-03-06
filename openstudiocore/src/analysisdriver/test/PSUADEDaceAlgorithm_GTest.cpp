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
#include <analysisdriver/test/AnalysisDriverFixture.hpp>

#include <analysisdriver/AnalysisDriver.hpp>
#include <analysisdriver/CurrentAnalysis.hpp>
#include <analysisdriver/AnalysisRunOptions.hpp>

#include <project/ProjectDatabase.hpp>
#include <project/AnalysisRecord.hpp>

#include <analysis/Analysis.hpp>
#include <analysis/Problem.hpp>
#include <analysis/DataPoint.hpp>
#include <analysis/PSUADEDaceAlgorithmOptions.hpp>
#include <analysis/PSUADEDaceAlgorithmOptions_Impl.hpp>
#include <analysis/PSUADEDaceAlgorithm.hpp>

#include <model/Model.hpp>

#include <runmanager/lib/RunManager.hpp>
#include <runmanager/lib/Workflow.hpp>
#include <runmanager/Test/ToolBin.hxx>

#include <utilities/document/Table.hpp>
#include <utilities/core/Optional.hpp>
#include <utilities/core/Path.hpp>
#include <utilities/core/PathHelpers.hpp>
#include <utilities/core/Checksum.hpp>
#include <utilities/core/FileReference.hpp>

#include <resources.hxx>
#include <OpenStudio.hxx>

#include <boost/foreach.hpp>
#include <boost/filesystem.hpp>

using namespace openstudio;
using namespace openstudio::model;
using namespace openstudio::ruleset;
using namespace openstudio::analysis;
using namespace openstudio::project;
using namespace openstudio::analysisdriver;

TEST_F(AnalysisDriverFixture, PSUADEDace_Continuous) {

  // RETRIEVE PROBLEM
  Problem problem = retrieveProblem("Continuous",true,false);

  // DEFINE SEED
  Model model = model::exampleModel();
  openstudio::path p = toPath("./example.osm");
  model.save(p,true);
  FileReference seedModel(p);

  // CREATE ANALYSIS
  PSUADEDaceAlgorithmOptions algOptions;
  Analysis analysis("PSUADEDace Continuous",
                    problem,
                    PSUADEDaceAlgorithm(algOptions),
                    seedModel);

  // RUN ANALYSIS
  ProjectDatabase database = getCleanDatabase("PSUADEDaceContinuous");
  AnalysisDriver analysisDriver(database);
  AnalysisRunOptions runOptions = standardRunOptions(analysisDriver.database().path().parent_path());
  CurrentAnalysis currentAnalysis = analysisDriver.run(analysis,runOptions);
  EXPECT_TRUE(analysisDriver.waitForFinished());
  boost::optional<runmanager::JobErrors> jobErrors = currentAnalysis.dakotaJobErrors();
  ASSERT_TRUE(jobErrors);
  EXPECT_TRUE(jobErrors->errors().empty());

  // output csv summary of data points
  Table summary = currentAnalysis.analysis().summaryTable();
  summary.save(analysisDriver.database().path().parent_path() / toPath("summary.csv"));

  BOOST_FOREACH(const DataPoint& dataPoint,analysis.dataPoints()) {
    EXPECT_TRUE(dataPoint.isComplete());
    EXPECT_FALSE(dataPoint.failed());
    // EXPECT_FALSE(dataPoint.responseValues().empty());
  }
}


TEST_F(AnalysisDriverFixture, PSUADEDace_MixedOsmIdf) {

  // RETRIEVE PROBLEM
  Problem problem = retrieveProblem("MixedOsmIdf",false,false);

  // DEFINE SEED
  Model model = model::exampleModel();
  openstudio::path p = toPath("./example.osm");
  model.save(p,true);
  FileReference seedModel(p);

  // CREATE ANALYSIS
  PSUADEDaceAlgorithmOptions algOptions;
  Analysis analysis("PSUADEDace Sampling",
                    problem,
                    PSUADEDaceAlgorithm(algOptions),
                    seedModel);

  // RUN ANALYSIS
  analysis = Analysis("PSUADEDace Sampling - MixedOsmIdf",
                      problem,
                      PSUADEDaceAlgorithm(algOptions),
                      seedModel);
  ProjectDatabase database = getCleanDatabase("PSUADEDace_MixedOsmIdf");
  AnalysisDriver analysisDriver = AnalysisDriver(database);
  AnalysisRunOptions runOptions = standardRunOptions(analysisDriver.database().path().parent_path());
  CurrentAnalysis currentAnalysis = analysisDriver.run(analysis,runOptions);
  EXPECT_TRUE(analysisDriver.waitForFinished());
  boost::optional<runmanager::JobErrors> jobErrors = currentAnalysis.dakotaJobErrors();
  ASSERT_TRUE(jobErrors);
  EXPECT_TRUE(jobErrors->errors().empty());
  EXPECT_TRUE(analysisDriver.currentAnalyses().empty());
  Table summary = currentAnalysis.analysis().summaryTable();
  summary.save(analysisDriver.database().path().parent_path() / toPath("summary.csv"));

  BOOST_FOREACH(const DataPoint& dataPoint,analysis.dataPoints()) {
    EXPECT_TRUE(dataPoint.isComplete());
    EXPECT_FALSE(dataPoint.failed());
  }

}
