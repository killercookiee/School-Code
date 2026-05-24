using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using NationalInstruments.DAQmx;
using System.IO;

namespace myDAQ2
{
    public partial class Form1 : System.Windows.Forms.Form
    {
        private NationalInstruments.DAQmx.Task InTask;
        private NationalInstruments.DAQmx.Task OutTask;
        private AnalogMultiChannelReader reader;
        private AnalogSingleChannelWriter writer;

        private int numSamples;
        private double sampleRate;
        private double Ag;

        private StreamWriter sw;
        private bool isSaving = false;

        public Form1()
        {
            InitializeComponent();
            // Initialize button states safely
            ConfigureCharts();

            Start_button.Enabled = true;
            Stop_button.Enabled = false;
        }

        private void ConfigureCharts()
        {
            // --- Chart 1: Drive Signal (Oscilloscope View) ---
            chart1.Titles.Clear();
            chart1.Titles.Add("Drive Signal (V_d)");
            chart1.ChartAreas[0].AxisX.Title = "Sample Index";
            chart1.ChartAreas[0].AxisY.Title = "Voltage (V)";
            chart1.ChartAreas[0].AxisY.Minimum = -10; // Match myDAQ hardware limits
            chart1.ChartAreas[0].AxisY.Maximum = 10;

            // --- Chart 2: Detected Signal (Oscilloscope View) ---
            chart2.Titles.Clear();
            chart2.Titles.Add("Detected Photodetector Signal (V_s)");
            chart2.ChartAreas[0].AxisX.Title = "Sample Index";
            chart2.ChartAreas[0].AxisY.Title = "Voltage (V)";
            chart2.ChartAreas[0].AxisY.Minimum = 0.25;
            chart2.ChartAreas[0].AxisY.Maximum = 0.35;

            // --- Chart 3: Multiplication Signal ---
            chart3.Titles.Clear();
            chart3.Titles.Add("Multiplication Signal (S = V_d * V_s)");
            chart3.ChartAreas[0].AxisX.Title = "Sample Index";
            chart3.ChartAreas[0].AxisY.Title = "Amplitude (V²)";

            // --- Chart 4: Lock-In Output (Historical Trend) ---
            chart4.Titles.Clear();
            chart4.Titles.Add("Calculated Amplitude of Modulation (A_led)");
            chart4.ChartAreas[0].AxisX.Title = "Elapsed Time (Measurement Points)";
            chart4.ChartAreas[0].AxisY.Title = "Modulation Amplitude (V)";
        }

        /// <summary>
        /// Safely terminates and disposes of all DAQmx hardware instances and timers.
        /// </summary>
        private void ClearTasks()
        {
            timer1.Stop();

            if (InTask != null)
            {
                try { InTask.Stop(); } catch { }
                InTask.Dispose();
                InTask = null;
            }

            if (OutTask != null)
            {
                try { OutTask.Stop(); } catch { }
                OutTask.Dispose();
                OutTask = null;
            }

            // Reset UI controls to non-running state
            Start_button.Enabled = true;
            Stop_button.Enabled = false;
        }

        private void Start_button_Click(object sender, EventArgs e)
        {
            ClearTasks(); // Ensure clean slate

            try
            {
                // Parse UI Input parameters
                numSamples = int.Parse(textBox_numsamples.Text);
                sampleRate = double.Parse(textBox_samplerate.Text);
                Ag = double.Parse(textBox_Ag.Text);

                // 1. Configure Analog Input Task (Continuous buffer to act as an oscilloscope)
                InTask = new NationalInstruments.DAQmx.Task();
                InTask.AIChannels.CreateVoltageChannel("myDAQ1/ai0", "", AITerminalConfiguration.Differential, -10, 10, AIVoltageUnits.Volts);
                InTask.AIChannels.CreateVoltageChannel("myDAQ1/ai1", "", AITerminalConfiguration.Differential, -10, 10, AIVoltageUnits.Volts);
                InTask.Timing.ConfigureSampleClock("", sampleRate, SampleClockActiveEdge.Rising, SampleQuantityMode.ContinuousSamples, numSamples * 10);

                // 2. Configure Analog Output Task (Continuous period reproduction)
                int period = 100;
                OutTask = new NationalInstruments.DAQmx.Task();
                OutTask.AOChannels.CreateVoltageChannel("myDAQ1/ao0", "", -10, 10, AOVoltageUnits.Volts);
                OutTask.Timing.ConfigureSampleClock("", sampleRate, SampleClockActiveEdge.Rising, SampleQuantityMode.ContinuousSamples, period);

                // 3. Bind Stream Readers/Writers
                reader = new AnalogMultiChannelReader(InTask.Stream);
                writer = new AnalogSingleChannelWriter(OutTask.Stream);

                // 4. Generate Square Excitation Waveform
                double[] wave = new double[period];
                for (int i = 0; i < period; i++)
                {
                    wave[i] = (i < period / 2) ? Ag : -Ag;
                }

                // Write to hardware buffer and start engines
                writer.WriteMultiSample(false, wave);

                OutTask.Start();
                InTask.Start();
                timer1.Start();

                // Toggle UI States to prevent multi-activation bugs
                Start_button.Enabled = false;
                Stop_button.Enabled = true;
            }
            catch (Exception ex)
            {
                ClearTasks();
                MessageBox.Show("Configuration Error: " + ex.Message, "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
            }
        }

        private void timer1_Tick(object sender, EventArgs e)
        {
            try
            {
                // Read a block of finite samples out of our continuous stream
                double[,] data = reader.ReadMultiSample(numSamples);
                double sumS = 0;

                // Clear oscilloscope charts for new frame refresh
                chart1.Series[0].Points.Clear();
                chart2.Series[0].Points.Clear();
                chart3.Series[0].Points.Clear();

                for (int i = 0; i < numSamples; i++)
                {
                    double Vd = data[0, i]; // Drive signal (AI0)
                    double Vs = data[1, i]; // Signal from detector (AI1)
                    double S = Vd * Vs;     // Mixed/Multiplied signal
                    sumS += S;

                    // Plot raw array indexes onto charts
                    chart1.Series[0].Points.AddY(Vd);
                    chart2.Series[0].Points.AddY(Vs);
                    chart3.Series[0].Points.AddY(S);

                    // If recording is active, stream raw structural rows to CSV file
                    if (isSaving && sw != null)
                    {
                        sw.WriteLine($"{Vd};{Vs};{S};");
                    }
                }

                // Lock-In Amplitude calculation principle: A_led = (2 * S_mean) / Ag
                double S_mean = sumS / numSamples;
                double A_led = (Ag != 0) ? (2 * S_mean) / Ag : 0;

                // Plot calculated metric onto historical trend chart (Chart 4)
                chart4.Series[0].Points.AddY(A_led);

                // Write Calculated Lock-in tracking point into file 
                if (isSaving && sw != null)
                {
                    sw.WriteLine($";;;{A_led}");
                }
            }
            catch (Exception ex)
            {
                ClearTasks();
                MessageBox.Show("Acquisition Runtime Error: " + ex.Message, "Runtime Crash", MessageBoxButtons.OK, MessageBoxIcon.Warning);
            }
        }

        private void Stop_button_Click(object sender, EventArgs e)
        {
            ClearTasks();

            if (isSaving && sw != null)
            {
                isSaving = false;
                sw.Close();
                sw.Dispose();
                sw = null;
                Save_button.BackColor = SystemColors.Control;
                Save_button.Text = "Save Data";
            }
        }

        private void Save_button_Click(object sender, EventArgs e)
        {
            if (!isSaving)
            {
                SaveFileDialog sfd = new SaveFileDialog();
                sfd.Filter = "CSV files (*.csv)|*.csv";

                if (sfd.ShowDialog() == DialogResult.OK)
                {
                    try
                    {
                        sw = new StreamWriter(sfd.FileName);
                        sw.AutoFlush = true;
                        // Initialize valid column headers
                        sw.WriteLine("V_d;V_s;Multiplication_S;A_led");

                        isSaving = true;
                        Save_button.BackColor = Color.LightGreen;
                        Save_button.Text = "Stop Saving";
                    }
                    catch (Exception ex)
                    {
                        MessageBox.Show("File Access Error: " + ex.Message, "File Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
                    }
                }
            }
            else
            {
                isSaving = false;
                if (sw != null)
                {
                    sw.Close();
                    sw.Dispose();
                    sw = null; // Clean pointer reference
                }
                Save_button.BackColor = SystemColors.Control;
                Save_button.Text = "Save Data";
                MessageBox.Show("Data file saved successfully.", "Saved", MessageBoxButtons.OK, MessageBoxIcon.Information);
            }
        }

        /// <summary>
        /// Overrides Windows Form exit event. Vital for DAQ systems to prevent driver lockups.
        /// </summary>
        protected override void OnFormClosing(FormClosingEventArgs e)
        {
            ClearTasks();
            if (sw != null)
            {
                try
                {
                    sw.Close();
                    sw.Dispose();
                }
                catch { }
            }
            base.OnFormClosing(e);
        }
    }
}