#!/usr/bin/env node

const Debug = require('debug');

/* eslint-disable no-unused-vars */
const log = Debug('fillEntries:log');
const error = Debug('fillEntries:error');
const debug = Debug('fillEntries:debug');
/* eslint-enable no-unused-vars */

const argv = require('yargs');

const moment = require('moment');

const xDripAPS = require('./xDripAPS')();

const snooze = ms => new Promise(resolve => setTimeout(resolve, ms));

const sgv = {
  readDate: '2019-01-05T22:14:41.494Z',
  readDateMills: 1546726481494,
  filtered: 170912,
  unfiltered: 168448,
  glucose: 174,
  nsNoise: 1,
  trend: -4.667351211511021,
  state: 0,
  g5calibrated: false,
};

let time = 0;
let numEntries = 0;

Debug.enable('fillEntries:*,xDripAPS:*');

if (argv.argv._[0]) {
  time = moment(argv.argv._[0]);
} else {
  error('Must provide an entry time to use');
  process.exit(1);
}

if (argv.argv._[1]) {
  numEntries = argv.argv._[1];
} else {
  error('Must provide the number of entries to upload');
  process.exit(1);
}

const fillEntries = async (entryTime, numEntries) => {
  for (; numEntries > 0; numEntries -= 1) {
    sgv.readDate = entryTime.valueOf();
    sgv.readDateMills = entryTime.valueOf();

    await snooze(10);

    xDripAPS.post(sgv, false);

    entryTime.add(1, 'minutes');
  }
};

fillEntries(time, numEntries);

