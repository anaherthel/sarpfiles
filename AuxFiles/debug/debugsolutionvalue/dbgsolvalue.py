
import pandas as pd
import math

def read_mat(file_path):
    # Reading the CSV file into a DataFrame
    df = pd.read_csv(file_path)
    
    # Converting the DataFrame to a matrix of floating point numbers
    matrix = df.values
    return matrix