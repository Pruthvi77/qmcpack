//////////////////////////////////////////////////////////////////////////////////////
// This file is distributed under the University of Illinois/NCSA Open Source License.
// See LICENSE file in top directory for details.
//
// Copyright (c) 2016 Jeongnim Kim and QMCPACK developers.
//
// File developed by: Mark Dewing, markdewing@gmail.com, University of Illinois at Urbana-Champaign
//
// File created by: Mark Dewing, markdewing@gmail.com, University of Illinois at Urbana-Champaign
//////////////////////////////////////////////////////////////////////////////////////


#define CATCH_CONFIG_RUNNER
#include "catch.hpp"
#include "DeviceManager.h"

#ifdef CATCH_MAIN_HAVE_MPI
#include "Message/Communicate.h"
#endif

// Replacement unit test main function to ensure that MPI is finalized once
// (and only once) at the end of the unit test.

// AFQMC specific unit test arguments.
std::string UTEST_HAMIL, UTEST_WFN;

int main(int argc, char* argv[])
{
  Catch::Session session;
  using namespace Catch::clara;
  // Build command line parser.
  auto cli = session.cli() |
      Opt(UTEST_HAMIL, "UTEST_HAMIL")["--hamil"]("Hamiltonian file to be used by unit test if applicable.") |
      Opt(UTEST_WFN, "UTEST_WFN")["--wfn"]("Wavefunction file to be used by unit test if applicable.");
  session.cli(cli);
  // Parse arguments.
  int parser_err = session.applyCommandLine(argc, argv);
#ifdef CATCH_MAIN_HAVE_MPI
  mpi3::environment env(argc, argv);
  OHMMS::Controller->initialize(env);
  Communicate node_comm;
  node_comm.initializeAsNodeComm(*OHMMS::Controller);
  // assign accelerators within a node
  qmcplusplus::DeviceManager::initializeGlobalDeviceManager(node_comm.rank(), node_comm.size());
#else
  qmcplusplus::DeviceManager::initializeGlobalDeviceManager(0, 1);
#endif
  // Run the tests.
  int result = session.run(argc, argv);
#ifdef CATCH_MAIN_HAVE_MPI
  OHMMS::Controller->finalize();
#endif
  if (parser_err != 0)
  {
    return parser_err;
  }
  else
  {
    return result;
  }
}
