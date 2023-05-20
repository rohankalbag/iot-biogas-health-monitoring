# Electronic Design Lab EE 344 IIT Bombay
# Team Tue: 23 
# Authors: Sankalp Bhamare, Rohan Rajesh Kalbag, Jujhaar Singh

# server backend

from flask import Flask, request, render_template
import pprint

class LoggingMiddleware(object):
    # class for middleware
    def __init__(self, app):
        self._app = app

    def __call__(self, env, resp):
        errorlog = env['wsgi.errors']
        pprint.pprint(('REQUEST', env), stream=errorlog)

        def log_response(status, headers, *args):
            pprint.pprint(('RESPONSE', status, headers), stream=errorlog)
            return resp(status, headers, *args)

        return self._app(env, log_response)

humidity = 0
co2_ppm = 0
seq_no = 0
app = Flask(__name__)

@app.route('/data', methods=['POST'])
def handle_data():
    # handle sensor data obtained from post request
    global humidity, co2_ppm, seq_no
    co2_ppm = int(request.headers.get('co2'))
    humidity = float(request.headers.get('humidity'))
    seq_no = seq_no + 1
    return "1234"

@app.route('/data', methods=['GET'])
def handle_data_send():
    # return dictionary representation of data received from sensors
    return {"co2_ppm" : co2_ppm, "humidity": humidity, "seq_no": seq_no}

@app.route('/')
def index_page():
    # visualize the plots and render html files
    return render_template('index.html', co2_ppm=co2_ppm, humidity=humidity)

if __name__ == '__main__':
    app.run(debug=True, host="0.0.0.0")
