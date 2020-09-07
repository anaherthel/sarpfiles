# -*- coding: utf-8 -*-
"""
Created on Fri Nov 10 09:06:24 2017

@author: bsarasol
"""

import matplotlib.pyplot as plt

def plot_coordinates(coordinates):
    x = [x for x, _ in coordinates]
    y = [y for _, y in coordinates]
    plt.plot(x, y, 'ro')
    plt.plot(x[-1], y[-1], 'gs')
    plt.plot(x[-2], y[-2], 'gs')
    plt.margins(0.2)
    plt.show()
    # Use plt.show() to plot in the spyder console.
    # Comment it out and uncomment one of the plt.savefig() lines
    # to save to a png/pdf file
    plt.show()
    # plt.savefig('map.png')
    # plt.savefig('map.pdf')  


def plot_route(route, coordinates):
    x = [x for x, _ in coordinates]
    y = [y for _, y in coordinates]    
    route_x = [x[i] for i in route]
    route_y = [y[i] for i in route]
    plt.plot(route_x, route_y, 'ro-')
#    plt.plot(route_x[0], route_y[0], 'gs')
    plt.plot(route_x[-1], route_y[-1], 'gs')
    plt.plot(route_x[-2], route_y[-2], 'gs')
    
    no_x = [x for i, (x, y) in enumerate(coordinates) if i not in route]
    no_y = [y for i, (x, y) in enumerate(coordinates) if i not in route]
    plt.plot(no_x, no_y, 'bx')
    plt.margins(0.2)
    plt.show()
    # Use plt.show() to plot in the spyder console.
    # Comment it out and uncomment one of the plt.savefig() lines
    # to save to a png/pdf file
    plt.show()
    # plt.savefig('route.png')
    # plt.savefig('route.pdf')  
    
    
def plot_route_with_labels(route, coordinates):
    labels = [label for label in range(len(coordinates))]
    x = [x for x, _ in coordinates]
    y = [y for _, y in coordinates]    
    route_x = [x[i] for i in route]
    route_y = [y[i] for i in route]
    plt.plot(route_x, route_y, 'ro-')
    plt.plot(route_x[-1], route_y[-1], 'gs')
    plt.plot(route_x[-2], route_y[-2], 'gs')
    no_x = [x for i, (x, y) in enumerate(coordinates) if i not in route]
    no_y = [y for i, (x, y) in enumerate(coordinates) if i not in route]
    plt.plot(no_x, no_y, 'bx')
    for i, c in enumerate(coordinates):
        x, y = c
        name = labels[i]
        plt.text(x, y, name, withdash=True,
           dashdirection=1,
           dashlength=5,
           dashpad=5,
           rotation=0,
           dashrotation=15,
           dashpush=10,
           )
    plt.margins(0.2)
    # Use plt.show() to plot in the spyder console.
    # Comment it out and uncomment one of the plt.savefig() lines
    # to save to a png/pdf file
    plt.show()
    # plt.savefig('route_labels.png')
    # plt.savefig('route_labels.pdf')    






