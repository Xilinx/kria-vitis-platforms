# Copyright 2019 Xilinx Inc.
# 
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
# 
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
import sys
if (sys.version_info > (3, 0)):
    PY3 = True
else:
    PY3 = False

import json
import logging
import time
import threading
if PY3:
    import urllib.request, urllib.error, urllib.parse
else:
    import urllib2
    import urlparse

try:
    import cv2
    import numpy as np
    import pafy
except ImportError as e:
    print(e)

from vai.dpuv1.rt.xsnodes.base import XstreamNode, XstreamNodeArgs

logger = logging.getLogger(__name__)

class Stream(object):
    def __init__(self, url, timeout_value):

        self.timeout = time.time()
        self.thread = None
        self.timeout_value = timeout_value
        self.stop = False
        self.url = url

    def is_stopped(self):
        return self.stop

    def is_timed_out(self):
        current_time = time.time()
        if current_time - self.timeout > self.timeout_value:
            return True
        return False

    def __str__(self):
        string = "*** Stream Object ***\n"
        string += "* Stop: " + str(self.stop) + "\n"
        string += "* Timeout: " + str(self.timeout) + "\n"
        string += "* Thread: " + str(self.thread) + "\n"
        string += "*********************"
        return string

class Node(XstreamNode):
    def _open_video(self, url):
        capture = cv2.VideoCapture(url)
        if capture.isOpened():
            framerate = capture.get(cv2.CAP_PROP_FPS)
            return (capture, framerate)
        video = None
        video_url = None
        # try to open the URL 5 times with Pafy (arbitrarily)
        for i in range(5):
            try:
                video = pafy.new(url)
            except Exception as e:
                print("A %s exception occurred: %s. Trying %d more times"
                    % (type(e).__name__, e.args, 9-i))
        if video is not None:
            video_url = video.streams[0].url
        # try to open the URL 5 times with custom code
        else:
            for i in range(5):
                try:
                    video_url = get_youtube_url(url)
                except Exception as e:
                    print("A %s exception occurred: %s. Trying %d more times"
                        % (type(e).__name__, e.args, 4-i))
        if video_url is not None:
            # print(video_url)
            capture = cv2.VideoCapture(video_url)
            if capture.isOpened():
                framerate = capture.get(cv2.CAP_PROP_FPS)
                return (capture, framerate)
        print("No video capture could be opened")
        return None

    def _execute(self, meta, capture, framerate, skip_count, index, counter, pub):

        status, image = capture.read()
        if status:
            meta['image_height'] = image.shape[0]
            meta['image_width'] = image.shape[1]
            meta['image_channels'] = image.shape[2]
            # print("Stream image valid")
            old_id = meta['id']

            # the first time, send the put with the original ID. This terminates
            # the service handler's request and lets the service continue through
            # callbacks
            if index != -1:
                meta['id'] = str(meta['id']) + "_" + str(index)
            # print("writing out of stream %s with count %s" % (meta['callback_id'], str(counter[0])))
            if (index == -1) or (index != -1 and index % skip_count == 0):
                # print("Sending out of stream %s" % meta['id'])
                self.put(meta, image, pub=pub)
                counter[0] += 1
            meta['id'] = old_id
        else:
            counter[1] += 1
            # print("Stream image invalid")

        time.sleep(1/float(framerate))

        # if skip_count >= 0:
        #     for i in range(skip_count):
        #         capture.grab()

        return (status, counter)

    def execute(self, stop, meta):
        retval = self._open_video(str(meta['url']))
        pub = self.get_pub()
        if retval:
            (capture, framerate) = retval
            # framerate = 15

            # run once in the non-looping case
            counter = [0, 0]
            frequency = self._frequency
            if frequency > framerate:
                frequency = framerate
            skip_count = int((framerate / float(frequency)) + 0.5)
            print("Using framerate of %d and sending every %dth frame (%s)" % (framerate, skip_count, meta['callback_id']))
            (status, counter) = self._execute(meta, capture, framerate, skip_count, -1, counter, pub)
            index = 0
            # print("Returned from first put")
            while not stop():
                try:
                    # print("*** New stream: %s ***" % time.time())
                    (status, counter) = self._execute(meta, capture, framerate, skip_count, index, counter, pub)
                    # if it failed to read, try opening the URL again
                    if not status:
                        (capture, framerate) = self._open_video(str(meta['url']))
                except:
                    logger.exception("stream error")
                index = (index+1) % 9999 # don't let IDs grow unbounded
            print("Valid frames %s | invalid frames %s (callback: %s)" %
                (str(counter[0]), str(counter[1]), meta['callback_id']))

    def check_threads(self, workers, meta_callback_id = "", meta_url = ""):
        delete_ids = []
        for callback_id, thread_obj in workers.items():
            if meta_callback_id == callback_id:
                thread_obj.timeout = time.time()
                if meta_url != "" and thread_obj.url != meta_url:
                    thread_obj.stop = True
                    thread_obj.thread.join()
                    print("stopped thread for callback_id %s" % callback_id)
                    delete_ids.append(callback_id)
            else:
                if thread_obj.is_timed_out():
                    thread_obj.stop = True
                    thread_obj.thread.join()
                    print("stopped thread for callback_id %s" % callback_id)
                    delete_ids.append(callback_id)
        for ids in delete_ids:
            del workers[ids]

    def initialize(self, args):
        self.workers = {}
        self.run()

    def finish(self):
        print("Got nothing, stream is ending")
        # i.e. if the thread has started
        for callback_id, thread_obj in self.workers.items():
            if isinstance(thread_obj.thread, threading.Thread):
                print("Stopping an existing thread")
                thread_obj.stop = True
                thread_obj.thread.join()
        print("All stream threads joined")
        super(Node, self).end()

    def timeout(self):
        self.check_threads(self.workers)
        return self.CONTINUE

    def loop(self, payload):
        (meta, buf) = payload

        # delete objects to decrement obj store refcount
        # -- this is so the object can be auto-deleted,
        #    so we can reuse the id in the put() below
        del buf
        del payload

        if self._frequency == -1:
            self.execute(lambda: True, meta)
        else:
            if self._timeout > 0:
                self.check_threads(self.workers, meta['callback_id'], meta['url'])

                if meta['callback_id'] not in self.workers:
                    print("Spawning thread for callback %s" % meta['callback_id'])
                    stream = Stream(meta['url'], self._timeout/1000.0)
                    worker = threading.Thread(
                        target=self.execute,
                        args=(stream.is_stopped, meta)
                    )
                    stream.thread = worker
                    self.workers[meta['callback_id']] = stream
                    worker.start()
                    # print("Finished spawning thread")

            else:
                print("Spawning thread for callback %s" % meta['callback_id'])
                stream = Stream(meta['url'], self._timeout/1000.0)
                worker = threading.Thread(
                    target=self.execute,
                    args=(stream.is_stopped, meta)
                )
                stream.thread = worker
                self.workers[meta['callback_id']] = stream
                worker.start()

def get_youtube_url(url):
    """
    This method extracts the *.m3u8 link for a YouTube live stream video. After
    further tests, this function isn't generic enough to use so it's deprecated.
    It's left here in case we ever want to break the dependency on pafy for som
    reason. *Note, this may return the wrong URL. The returned URL differs from
    the one from Pafy

    Args:
        url (str): YouTube URL

    Returns:
        str: a URL that should be opened with OpenCV.VideoCapture()
    """
    youtube_prefix = "https://www.youtube.com/get_video_info?&video_id="
    video_id = url[-11:]
    video_url = youtube_prefix + video_id
    if PY3:
        response = urllib.request.urlopen(video_url)
    else:
        response = urllib2.urlopen(video_url)
    r = response.read()

    if PY3:
        response = json.loads(str(json.dumps(urllib.parse.parse_qs(r))))
    else:
        response = json.loads(str(json.dumps(urlparse.parse_qs(r))))
    player_response = json.loads(response['player_response'][0])

    # video_url = player_response['streamingData']['hlsManifestUrl']

    if 'streamingData' in player_response:
        formats = player_response['streamingData']['formats']
    else:
        return None
    # print(formats)
    mp4_videos = []
    for video in formats:
        # print(video)
        if video['mimeType'].startswith('video/mp4'):
            mp4_videos.append(video)

    videos_sorted = sorted(mp4_videos, key = lambda i: i['width'])
    video = videos_sorted[0]

    video_url = None
    if 'url' in video:
        video_url = video['url']
    else:
        # for some reason, some videos' responses get garbled, either by the
        # parser or by Youtube. There's a URL field but it's often inside a
        # 'cipher' key instead of being by itself. When I tried to extract the
        # field, the resulting URL is apparently invalid. In youtube-dl, there's
        # some code for cipher when parsing the URL so maybe this is a known
        # problem.
        # for key, value in video.items():
        #     found_index = str(value).find('url=')
        #     if found_index != -1:
        #         video_url = str(urllib2.unquote(video[key][found_index+4:]))
        #         break
        return None

    return video_url

class Args(XstreamNodeArgs):
    def __init__(self):
        super(Args, self).__init__()

        self.add_start_arg(
            'timeout',
            "(int, optional): If timeout > 0 && frequency > -1, then keepalive " +
                "messages from clients are expected every x ms or the streaming " +
                "thread for it is closed"
        )
        self.add_start_arg(
            'callback',
            "(bool, situational): Should be True if frequency > -1. Setting " +
                "callback to True, and providing a callback_id, sends the meta " +
                "over the __server__ channel to be sent over a websocket to the " +
                "callback_id from the current node. Defaults to False."
        )
        self.add_start_arg(
            'frequency',
            "(int, optional): FPS of the video to use. Frames in between are dropped." +
                "If this is -1, then only the first frame would be processed"
        )

        self.add_input_serve_arg(
            'BUFFER',
            "Not used"
        )
        self.add_input_serve_arg(
            'url',
            "(str): a URL to a video to stream"
        )
        self.add_input_serve_arg(
            'dtype',
            "(str, situational): Used if mode=(url|image) to specify the attribute " +
                "from np to read the data as e.g. uint8 or float32"
        )

        self.add_output_serve_arg(
            'image_height',
            "Height of the frame in pixels"
        )
        self.add_output_serve_arg(
            'image_width',
            "Width of the frame in pixels"
        )
        self.add_output_serve_arg(
            'image_channels',
            "Channels in the frame (usually should be 3)"
        )
        self.add_output_serve_arg(
            'BUFFER',
            "Current frame of video stream is put in the buffer"
        )
