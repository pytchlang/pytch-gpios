import json


def set_input_message_str(seqnum, pin, pull_kind):
    command = {
        "seqnum": seqnum,
        "kind": "set-input",
        "pin": pin,
        "pullKind": pull_kind,
    }
    return json.dumps([command])
