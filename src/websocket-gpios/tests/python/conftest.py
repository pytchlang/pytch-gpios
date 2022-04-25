import pytest
import json


@pytest.fixture(scope="session")
def reset_cmd_obj():
    return {"seqnum": 1234, "kind": "reset"}


@pytest.fixture(scope="session")
def reset_message_obj(reset_cmd_obj):
    return [reset_cmd_obj]


@pytest.fixture(scope="session")
def reset_message_str(reset_message_obj):
    return json.dumps(reset_message_obj)
