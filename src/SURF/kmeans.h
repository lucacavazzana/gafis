#include <vector>
#include <time.h>
#include <stdlib.h>
#include "surflib.h"

//-----------------------------------------------------------
// Kmeans clustering class (under development)
//  - Can be used to cluster points based on their location.
//  - Create Kmeans object and call Run with IpVec.
//  - Planned improvements include clustering based on motion 
//    and descriptor components.
//-----------------------------------------------------------

class Kmeans {

public:

  //! Destructor
  ~Kmeans() {};

  //! Constructor
  Kmeans() {};

  //! Do it all!
  void Run(IpVec &ipts, int clusters, bool init = false);

  //! Set the ipts to be used
  void SetIpoints(IpVec ipts);

  //! Randomly distribute 'n' clusters
  void InitRandomClusters(int n);

  //! Assign Ipoints to clusters
  bool AssignToClusters();

  //! Calculate new cluster centers
  void RepositionClusters();

  //! Function to measure the distance between 2 ipoints
  float Distance(Ipoint &ip1, Ipoint &ip2);

  //! Vector stores ipoints for this run
  IpVec ipts;

  //! Vector stores cluster centers
  IpVec clusters;

};

//-------------------------------------------------------

void Kmeans::Run(IpVec &ipts, int clusters, bool init)
{
  if (!ipts.size()) return;

  SetIpoints(ipts);

  if (init) InitRandomClusters(clusters);
  
  while (AssignToClusters())
  {
    RepositionClusters();
  }
}

//-------------------------------------------------------

void Kmeans::SetIpoints(IpVec ipts)
{
  this->ipts = ipts;
}

//-------------------------------------------------------

void Kmeans::InitRandomClusters(int n)
{
  // clear the cluster vector
  clusters.clear();

  // Seed the random number generator
  srand(time(NULL));

  // add 'n' random ipoints to clusters list as initial centers
  for (int i = 0; i < n; ++i)
  {
    clusters.push_back(ipts.at(rand() % ipts.size()));
  }
}

//-------------------------------------------------------

bool Kmeans::AssignToClusters()
{
  bool Updated = false;

  // loop over all Ipoints and assign each to closest cluster
  for (unsigned int i = 0; i < ipts.size(); ++i)
  {
    float bestDist = FLT_MAX;
    int oldIndex = ipts[i].clusterIndex;

    for (unsigned int j = 0; j < clusters.size(); ++j)
    {
      float currentDist = Distance(ipts[i], clusters[j]);
      if (currentDist < bestDist)
      {
        bestDist = currentDist;
        ipts[i].clusterIndex = j;
      }
    }

    // determine whether point has changed cluster
    if (ipts[i].clusterIndex != oldIndex) Updated = true;
  }

  return Updated;
}

//-------------------------------------------------------

void Kmeans::RepositionClusters()
{
  int x, y, count;

  for (unsigned int i = 0; i < clusters.size(); ++i)
  {
    x = y = 0;
    count = 1;

    for (unsigned int j = 0; j < ipts.size(); ++j)
    {
      if (ipts[j].clusterIndex == i)
      {
        x += ipts[j].x;
        y += ipts[j].y;
        ++count;
      }
    }

    clusters[i].x = x/count;
    clusters[i].y = y/count;
  }
}

//-------------------------------------------------------

float Kmeans::Distance(Ipoint &ip1, Ipoint &ip2)
{
  return sqrt(pow(ip1.x - ip2.x,2) 
        + pow(ip1.y - ip2.y, 2));;
}

//-------------------------------------------------------
