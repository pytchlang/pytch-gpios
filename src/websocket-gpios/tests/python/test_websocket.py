import websockets
import json

import pytest


def set_input_message_str(seqnum, pin, pull_kind):
    command = {
        "seqnum": seqnum,
        "kind": "set-input",
        "pin": pin,
        "pullKind": pull_kind,
    }
    return json.dumps([command])


def set_output_message_str(seqnum, pin, level):
    command = {
        "seqnum": seqnum,
        "kind": "set-output",
        "pin": pin,
        "level": level,
    }
    return json.dumps([command])


@pytest.mark.asyncio
async def test_connection():
    async with websockets.connect("ws://localhost:8055/") as ws:
        pass
