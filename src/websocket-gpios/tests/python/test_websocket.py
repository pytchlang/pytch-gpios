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


@pytest.mark.asyncio
async def test_exactly_one_connection_succeeds(reset_message_str):
    n_clients = 4
    connections = [
        websockets.connect("ws://localhost:8055/")
        for _ in range(n_clients)
    ]
    clients = await asyncio.gather(*connections)

    await asyncio.gather(*[c.send(reset_message_str) for c in clients])

    reply_objs = [
        (await collect_replies(c, 1, keep_unsolicited=False))[0]
        for c in clients
    ]

    ok_idxs = [
        i for (i, r) in enumerate(reply_objs) if r[0]["kind"] == "ok"
    ]
    assert len(ok_idxs) == 1

    n_errors = sum(1 for r in reply_objs if r[0]["kind"] == "error")
    assert n_errors == n_clients - 1

    # If we close the successful client, we should be able to
    # get a new real connection.

    await clients[ok_idxs[0]].close()
    del clients[ok_idxs[0]]

    new_client = await websockets.connect("ws://localhost:8055/")
    clients.append(new_client)
    await new_client.send(reset_message_str)
    reply_obj = (await collect_replies(new_client, 1))[0]
    assert_sole_ok(reply_obj, 1234)

    await asyncio.gather(*[c.close() for c in clients])


def assert_contains_once(replies, seqnum, kind):
    msgs = reduce(concat, replies, [])
    n_matching = sum(
        1 for m in msgs if m["seqnum"] == seqnum and m["kind"] == kind
    )
    assert n_matching == 1, f"no match for {seqnum}/{kind}"
