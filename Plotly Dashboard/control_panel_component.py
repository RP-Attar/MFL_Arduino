# control_panel_component.py

from dash import Dash, html, dcc,Input, Output, callback
from numpy import NaN
import plotly.express as px
import pandas as pd
import math

from app import app

# create an encapsulated layout component
control_panel = html.Div([
    html.H2('Select Data'),

    html.Div([
        html.P('Enter file path: '),
    ]),
    
    html.Div([
        dcc.Input(
            id='input_filePath',
            placeholder='Paste file path...',
            type='text',
            value=''
        ),
    ]),

    html.Div(dcc.Graph(id='preview_xy')),
])

##### Load and store average data based on filePath input #####
@callback(
    Output('chosen_data', 'data'),
    Input('input_filePath', 'value')
)
def saveData(input_text):
    if '.txt' in input_text:
        df = pd.read_csv(input_text, sep=",", header=None, names=['steps','usec','hall1','hall2'])
        return df.to_dict() # must store as JSONifieable object (eg/ dict) - convert back to df when using


##### Create plot of both Halls as preview of data #####
@callback(
    Output('preview_xy', 'figure'),
    Input('chosen_data', 'data')
)
def plotPreview(data):
    if data != ' ':
        # get data from storage and put into df
        df = pd.DataFrame.from_dict(data, orient='columns')
        
        # create 'time' row
        timeStep = math.floor(df['usec'].mean())
        time = [0]
        for i in range(len(df)-1):
            time.append(time[i]+timeStep)
        df['time'] = time

        # create figure
        fig = px.line(df, x='time', y='hall1')
        return fig