/*
  Stockfish, a UCI chess playing engine derived from Glaurung 2.1
  Copyright (C) 2004-2008 Tord Romstad (Glaurung author)
  Copyright (C) 2008 Marco Costalba

  Stockfish is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.
  
  Stockfish is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.
  
  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/


////
//// Includes
////
#include <fstream>
#include <sstream>
#include <vector>

#include "benchmark.h"
#include "search.h"
#include "thread.h"
#include "ucioption.h"


////
//// Variables
////

const std::string BenchmarkPositions[] = {
  "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1",
  "r4rk1/1b2qppp/p1n1p3/1p6/1b1PN3/3BRN2/PP3PPP/R2Q2K1 b - - 7 16",
  "4r1k1/ppq3pp/3b4/2pP4/2Q1p3/4B1P1/PP5P/R5K1 b - - 0 20",
  "4rrk1/pp1n3p/3q2pQ/2p1pb2/2PP4/2P3N1/P2B2PP/4RRK1 b - - 7 19",
  "rq3rk1/ppp2ppp/1bnpb3/3N2B1/3NP3/7P/PPPQ1PP1/2KR3R w - - 7 14",
  "r1bq1r1k/1pp1n1pp/1p1p4/4p2Q/4Pp2/1BNP4/PPP2PPP/3R1RK1 w - - 2 14",
  "r3r1k1/2p2ppp/p1p1bn2/8/1q2P3/2NPQN2/PPP3PP/R4RK1 b - - 2 15",
  "r1bbk1nr/pp3p1p/2n5/1N4p1/2Np1B2/8/PPP2PPP/2KR1B1R w kq - 0 13",
  "r1bq1rk1/ppp1nppp/4n3/3p3Q/3P4/1BP1B3/PP1N2PP/R4RK1 w - - 1 16",
  "4r1k1/r1q2ppp/ppp2n2/4P3/5Rb1/1N1BQ3/PPP3PP/R5K1 w - - 1 17",
  "2rqkb1r/ppp2p2/2npb1p1/1N1Nn2p/2P1PP2/8/PP2B1PP/R1BQK2R b KQ - 0 11",
  "r1bq1r1k/b1p1npp1/p2p3p/1p6/3PP3/1B2NN2/PP3PPP/R2Q1RK1 w - - 1 16",
  "3r1rk1/p5pp/bpp1pp2/8/q1PP1P2/b3P3/P2NQRPP/1R2B1K1 b - - 6 22",
  "r1q2rk1/2p1bppp/2Pp4/p6b/Q1PNp3/4B3/PP1R1PPP/2K4R w - - 2 18",
  "4k2r/1pb2ppp/1p2p3/1R1p4/3P4/2r1PN2/P4PPP/1R4K1 b  - 3 22",
  "3q2k1/pb3p1p/4pbp1/2r5/PpN2N2/1P2P2P/5PP1/Q2R2K1 b - - 4 26"
};
  

////
//// Functions
////

/// benchmark() runs a simple benchmark by letting Stockfish analyze a set
/// of positions for a given time each.  There are four parameters; the
/// transposition table size, the number of search threads that should
/// be used, the time in seconds spent for each position (optional, default
/// is 60) and an optional file name where to look for positions in fen
/// format (default are the BenchmarkPositions defined above).
/// The analysis is written to a file named bench.txt.

void benchmark(const std::string& commandLine) {

  std::istringstream csVal(commandLine);
  std::istringstream csStr(commandLine);
  std::string ttSize, threads, fileName;
  int val, secsPerPos;

  csStr >> ttSize;
  csVal >> val;
  if (val < 4 || val > 1024)
  {
      std::cerr << "The hash table size must be between 4 and 1024" << std::endl;
      exit(EXIT_FAILURE);
  }
  csStr >> threads;
  csVal >> val;
  if (val < 1 || val > THREAD_MAX)
  {
      std::cerr << "The number of threads must be between 1 and " << THREAD_MAX
                << std::endl;
      exit(EXIT_FAILURE);
  }  
  set_option_value("Hash", ttSize);
  set_option_value("Threads", threads);
  set_option_value("OwnBook", "false");
  set_option_value("Use Search Log", "true");
  set_option_value("Search Log Filename", "bench.txt");

  csVal >> secsPerPos;
  csVal >> fileName;

  std::vector<std::string> positions;
  
  if (fileName != "default")
  {
      std::ifstream fenFile(fileName.c_str());
      if (!fenFile.is_open())
      {
          std::cerr << "Unable to open positions file " << fileName
                    << std::endl;
          exit(EXIT_FAILURE);
      }        
      std::string pos;
      while (fenFile.good())
      {
          std::getline(fenFile, pos);
          if (!pos.empty())
              positions.push_back(pos);
      }
      fenFile.close();
  } else
      for (int i = 0; i < 16; i++)
          positions.push_back(std::string(BenchmarkPositions[i]));

  std::vector<std::string>::iterator it;
  for (it = positions.begin(); it != positions.end(); ++it)
  {
      Move moves[1] = {MOVE_NONE};
      int dummy[2] = {0, 0};
      Position pos(*it);
      think(pos, true, false, 0, dummy, dummy, 0, 0, 0, secsPerPos * 1000, moves);
  }
}
