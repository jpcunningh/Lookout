#!/usr/bin/env node

const Debug = require('debug');

/* eslint-disable no-unused-vars */
const log = Debug('transmitterIO:log');
const error = Debug('transmitterIO:error');
const debug = Debug('transmitterIO:debug');
/* eslint-enable no-unused-vars */

const argv = require('yargs');

const moment = require('moment');
const storage = require('node-persist');
const calibration = require('./calibration');


const getUnfiltered = async (time) => {
  await storage.init({ dir: `${__dirname}/storage` });

  const glucoseHist = await storage.getItem('glucoseHist')
    .catch((err) => {
      error(`Error getting glucoseHist: ${err}`);
    });

  const unfiltered = await calibration.getUnfiltered(time, glucoseHist, null);

  log(`Unfiltered: ${unfiltered}`);
};

const time = moment(argv.argv._[0]);

log(`Getting unfiltered value for time: ${time.format()}`);

getUnfiltered(time);
