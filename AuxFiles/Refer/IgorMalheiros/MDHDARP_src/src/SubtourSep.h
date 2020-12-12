#ifndef _SUBTOUR_SEP_H_
#define _SUBTOUR_SEP_H_

#include <vector>

class SubtourSep
{
    public:
      std::vector<std::vector<int>> separate(std::vector<std::vector<double>> &x_vals, int n);

      std::vector<std::vector<int>> separateInteger(std::vector<std::vector<int>> &x_vals, int n);

      std::vector<std::vector<int>> maxback(std::vector<std::vector<double>> &x_vals, int n);
};

#endif
