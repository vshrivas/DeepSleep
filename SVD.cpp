#include<iostream>
#include<fstream>
#include<string>
#include<numeric>
#include<math.h>
using namespace std;

// these are all the global variables used by the program

// users and movies are one indexed
const double numUsers = 458293;
const double numMovies = 17770;
const double numPts = 102416306;

// K is the constant representing the number of features
// lrate is the learning rate
const double K = 5;
const double lrate = 0.001;

// these 2D arrays are the U and V in the SVD
double **userValues;
double **movieValues;

// 2D arrays to store ratings and indexes
double **ratings;
double *indexes;

void readRatingsIndexes()
{
  cout << "Reading in the training data." << "\n";

  // read in data
  // change file paths as necessary
  fstream ratingsFile ("../um/all.dta");
  fstream indexFile ("../um/all.idx");
  // user, movie, date, rating
  double inputs [4] = {};
  int index;
  int pt = 0;

  if (ratingsFile.is_open() && indexFile.is_open()) {
    cout << "Files are open in readRatingsIndexes.\n" << "\n";
    while (ratingsFile >> inputs[0] >> inputs[1] >> inputs[2] >> inputs[3]) {
        indexFile >> index;
        indexes[pt] = index;

        double user = inputs[0];
        double movie = inputs[1];
        double date = inputs[2];
        double rating = inputs[3];
        ratings[pt][0] = user;
        ratings[pt][1] = movie;
        ratings[pt][2] = date;
        ratings[pt][3] = rating;

        // increment pt so that we update the values for a new point next time
        pt += 1;
    }
  }


  ratingsFile.close();
  indexFile.close();
}

// given a user and a movie this function gives the predicted rating
double predictRating(int user, int movie)
{
  double rating = 0;
  for (int i = 0; i < K; i++)
  {
    rating += userValues[user][i] * movieValues[movie][i];
  }
  return rating;
}


double error ()
{
  cout << "Calculating Validation error..." << "\n";

  //  counter keeps track of the number of points we've been through
  int counter = 0;
  double error = 0;
  double diff;
  double index;

  for (int i = 0; i < numPts; i++) {
      index = indexes[i];

      if (index == 2) {
          diff = ratings[i][3] - predictRating(ratings[i][0], ratings[i][1]);
      }
      error += diff * diff;
      counter++;
  }

  return sqrt(error);
}

void train(int user, int movie, int rating)
{
  // calculate the error with the current feature values
	double err = lrate * ((double) rating - predictRating(user, movie));

  // updates the movie and user vectors feature by feature
  double *uv = userValues[user];
  for (int i = 0; i < K; i++){
    userValues[user][i] += err * movieValues[movie][i];
    movieValues[movie][i] += err * uv[i];
  }
}

void initialize()
{
  cout << "Initializing the program.\n";
  // allocate and initialize the userValues and movieValues matrices
  // all of the +1 terms result from the fact that these arrays are 1
  // indexed in the data
  userValues = new double*[((int) numUsers + 1)];
  for (int i = 0; i < numUsers + 1; i++)
  {
    userValues[i] = new double[(int) K];
    for (int j = 0; j < K; j++)
    {
      userValues[i][j] = 0.1;
    }
  }

  movieValues = new double*[((int) numMovies + 1)];
  for (int i = 0; i < numMovies + 1; i++)
  {
    movieValues[i] = new double[(int) K];
    for (int j = 0; j < K; j++)
    {
      movieValues[i][j] = 0.1;
    }
  }
 cout << "Allocating ratings array\n";
  // create  the arrays that store the ratings input data and the indexes
  ratings = new double* [((int) numPts)];
  for(int i = 0; i < numPts; i++)
  {
      ratings[i] = new double [4];
  }
  cout << "Done Allocating ratings array\n";
  indexes = new double [((int) numPts)];
}

void cleanUp()
{
  // de-allocate user values and movie values to prevent memory leak
  for (int i = 0; i < numUsers + 1; i++)
  {
    delete [] userValues[i];
  }
  delete [] userValues;
  for (int i = 0; i < numMovies + 1; i++)
  {
    delete [] movieValues[i];
  }
  delete [] movieValues;

  for (int i = 0; i < numPts; i++) {
    delete[] ratings[i];
  }
  delete[] ratings;

  delete[] indexes;
}

void runEpoch ()
{
  cout << "Running Epoch..." << "\n";

  int counter = 0;

  for (int i = 0; i < numPts; i++) {
      int index = indexes[i];

      if (index == 1) {
          train(ratings[i][0], ratings[i][1], ratings[i][3]);
      }

      counter++;
      // print the counter every 1000000 points; I don't print every point
      // because that slows it down
      if (counter % 1000000 == 0)
      {
        cout << counter << "\n";
      }
  }
}

// Opens the file and runs the SVD
int main()
{
  initialize();
  readRatingsIndexes();
  // gets the initial validation error
  // NOTE: the initial error should calculate the error in the final program
  double initialError = 1;
  double finalError = 10;
  cout << "Initial Error is: " << initialError << "\n";
  cout << "Final Error is: " << finalError << "\n";
  // NOTE: Uncomment the while loop once we've optimized the single epoch stuff
  // while (initialError/finalError > 0.01) {
    initialError = finalError;
    runEpoch();
    finalError = error();
    cout << "Initial Error is: " << initialError << "\n";
    cout << "Final Error is: " << finalError << "\n";
  // }
  cleanUp();
  return 0;
}
