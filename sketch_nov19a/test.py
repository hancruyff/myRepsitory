import serial
import sys
from sdk.api.message import Message
from sdk.exceptions import CoolsmsException
ser = serial.Serial("COM5", 9600)
while True:
    C=int(ser.readline().decode('utf-8'))
    
    if(C==1):
        if __name__ == "__main__":

    # set api key, api secret
            api_key = "NCSAYSLBEJOJNAWP"
            api_secret = "G9O5C4RF0KRP5DGYABD1H0RVV5UD8OVT"

    ## 4 params(to, from, type, text) are mandatory. must be filled
            params = dict()
            params['type'] = 'sms' # Message type ( sms, lms, mms, ata )
            params['to'] = '01024565604 ' # Recipients Number '01000000000,01000000001'
            params['from'] = '01024565604' # Sender number
            params['text'] = '사고발생' # Message

            cool = Message(api_key, api_secret)
            try:
                response = cool.send(params)
                print("Success Count : %s" % response['success_count'])
                print("Error Count : %s" % response['error_count'])
                print("Group ID : %s" % response['group_id'])

                if "error_list" in response:
                    print("Error List : %s" % response['error_list'])

            except CoolsmsException as e:
                print("Error Code : %s" % e.code)
                print("Error Message : %s" % e.msg)

            sys.exit()
