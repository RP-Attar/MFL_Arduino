# index.py

from dash import Dash, html, dcc
import plotly.express as px
import pandas as pd

from app import app
from control_panel_component import *

app.layout = html.Div(children=[
    html.Div([
        html.H1('MFL Data Dashboard')
    ]),

    # Panel for manipulating and selecting data
    control_panel, 

    # Place for manipulated data to be stored
    dcc.Store(id='chosen_data'),
])

if __name__ == '__main__':
    app.run_server(debug=True)