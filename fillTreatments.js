#!/usr/bin/env node

const Debug = require('debug');

/* eslint-disable no-unused-vars */
const log = Debug('fillTreatments:log');
const error = Debug('fillTreatments:error');
const debug = Debug('fillTreatments:debug');
/* eslint-enable no-unused-vars */

const argv = require('yargs');

const moment = require('moment');

const xDripAPS = require('./xDripAPS')();

const snooze = ms => new Promise(resolve => setTimeout(resolve, ms));

let entryTime = 0;
let numEntries = 0;

Debug.enable('fillTreatments:*,xDripAPS:*');

if (argv.argv._[0]) {
  entryTime = moment(argv.argv._[0]);
} else {
  error('Must provide an entry time to use');
  process.exit(1);
}

if (argv.argv._[1]) {
  [, numEntries] = argv.argv._;
} else {
  error('Must provide the number of entries to upload');
  process.exit(1);
}

const fillTreatments = async () => {
  for (; numEntries > 0; numEntries -= 1) {
    /* eslint-disable-next-line no-await-in-loop */
    await snooze(10);

    xDripAPS.postEvent('Test Event', entryTime);

    entryTime.add(1, 'minutes');
  }
};

fillTreatments();
