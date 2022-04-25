import asyncio
from functools import reduce
from operator import concat
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


async def collect_replies(client, n_replies, keep_unsolicited=False):
    replies = []
    while len(replies) < n_replies:
        reply_str = await asyncio.wait_for(client.recv(), 0.5)
        reply = json.loads(reply_str)
        is_unsolicited = len(reply) == 1 and reply[0]["seqnum"] == 0
        if keep_unsolicited or not is_unsolicited:
            replies.append(reply)
    return replies


@pytest.mark.asyncio
async def test_connection():
    async with websockets.connect("ws://localhost:8055/") as ws:
        pass


def assert_sole_ok(reply_obj, exp_seqnum):
    assert isinstance(reply_obj, list)
    assert len(reply_obj) == 1
    response_obj = reply_obj[0]
    assert response_obj["seqnum"] == exp_seqnum
    assert response_obj["kind"] == "ok"


@pytest.mark.asyncio
async def test_reset(reset_message_str):
    async with websockets.connect("ws://localhost:8055/") as ws:
        await ws.send(reset_message_str)
        reply_obj = (await collect_replies(ws, 1))[0]
        assert_sole_ok(reply_obj, 1234)
