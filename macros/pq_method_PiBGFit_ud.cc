#include <iostream>
#include <vector>

using std::cout; using std::endl;
using std::vector;

Float_t bins_cos_fine[] = {-1.0,-0.98,-0.96,-0.94,-0.92,-0.90,-0.88,-0.86,-0.84,-0.82,-0.80,-0.75,-0.70,-0.60,-0.50,-0.40,-0.30,-0.20,-0.10,
                            0.0,0.10,0.20,0.30,0.40,0.50,0.60,0.70,0.75,0.80,0.82,0.84,0.86,0.88,0.90,0.92,0.94,0.96,0.98,1.0};
// Int_t   nbins_cos = sizeof(bins_cos_fine) / sizeof(Float_t) - 1;
Int_t   nbins_cos = 100;

Float_t bins_cos_fine_half[] = {0.0,0.10,0.20,0.30,0.40,0.50,0.60,0.70,0.75,0.80,0.82,0.84,0.86,0.88,0.90,0.92,0.94,0.96,0.98,1.0};
Int_t   nbins_cos_half = sizeof(bins_cos_fine_half) / sizeof(Float_t) - 1;

void BinNormal(TH1F *h)
{
  const Int_t nbins = h->GetNbinsX();
  for (int ibin=1; ibin<=nbins; ibin++){
    Float_t binc = h->GetBinContent(ibin);
    Float_t binw = h->GetBinWidth(ibin);
    Float_t bin_normal = binc / binw;
    h->SetBinContent(ibin,bin_normal);
  }

}

void Normalize(TH1F *h, Float_t norm_top)
{
  // h->Scale( 1.0 / h->GetEntries() );
  const Int_t nbins = h->GetNbinsX();
  Int_t nbins4 = nbins / 4;
  // Int_t int_high = (nbins / 2) + nbins4;
  // Int_t int_low  = (nbins / 2 + 1) - nbins4;
  // Int_t int_high = nbins-4;
  // Int_t int_low  = (nbins / 2);
  Int_t int_high = nbins-4;
  Int_t int_low  = (nbins / 2) + 4;
  h->Scale( norm_top / h->Integral(int_low,int_high) );
}

void NormalizeUU(TH1F *h, Float_t norm_top)
{
  const Int_t nbins = h->GetNbinsX();
  Int_t nbins4 = nbins / 4;
  Int_t int_high = nbins;
  Int_t int_low  = 1;

  cout << "uu integral = " << h->Integral(int_low,int_high) << endl;
  cout << "uu entries  = " << h->GetEntries() << endl;

  h->Scale( norm_top / h->Integral(int_low,int_high) );

}

void NormalizeGen(TH1F *h, Float_t norm_top)
{
  h->Scale( norm_top / h->GetEntries() );
}

void Normalize2Reco(TH1F *h_reco, TH1F *h_gen)
{
	double intCosReco = h_reco->Integral(20,80);
	double intCosGen  = h_gen->Integral(20,80);
  h_gen->Scale( intCosReco / intCosGen );
}

void StyleHist(TH1F *h, Color_t col)
{
  h->SetLineWidth(3);
  h->SetLineColor(col);
  h->SetFillStyle(3002);
  h->SetFillColor(col);
}

void StylePad(TPad *pad, Float_t t, Float_t b, Float_t r, Float_t l)
{
  pad->SetGrid(1,1);
  if(t) pad->SetTopMargin(t);
  if(b) pad->SetBottomMargin(b);
  if(r) pad->SetRightMargin(r);
  if(l) pad->SetLeftMargin(l);
  pad->Draw();
  pad->cd();

}

vector<Float_t> GetP( TH1F * h_accepted, TH1F * h_rejected )
{
  const Int_t nbins = h_accepted->GetNbinsX();
  vector<Float_t> result_error;
  vector<Float_t> result;

  for (int i = 1; i < nbins / 2 + 1; i++)
  {
    std::vector<float> result_j;
    for (int i1 = -1; i1 < 2; i1 += 2)
    {
      for (int i2 = -1; i2 < 2; i2 += 2)
      {
        for (int i3 = -1; i3 < 2; i3 += 2)
        {
          float accepted = h_accepted->GetBinContent(nbins + 1 - i) + i1 * sqrt(h_accepted->GetBinContent(nbins + 1 - i));
          float rejected = h_rejected->GetBinContent(nbins + 1 - i) + i2 * sqrt(h_rejected->GetBinContent(nbins + 1 - i));
          accepted += h_accepted->GetBinContent(i) + i3 * sqrt(h_accepted->GetBinContent(i));

          float a = 1;
          float b = -1;
          float c = rejected / (2 * (accepted + rejected));
          float p = (0.5 / a) * (-b + sqrt(b * b - 4 * a * c));
          float p2 = (0.5 / a) * (-b - sqrt(b * b - 4 * a * c));
          if (p > 0.99)
            p = 0;
          if (p2 > 0.99)
            p2 = 0;
          if (p > 0 || p2 > 0)
            result_j.push_back(max(p, p2));
        }
      }
    }

    float average = 0;
    float n = 0;
    for (unsigned j = 0; j < result_j.size(); j++)
    {
      if (result_j.at(j) > 0)
      {
        average += result_j.at(j);
        n++;
      }
    }
    average /= n;

    if ( (average != 0) && (average == average) )
    {
      result.push_back(average);
      float std_dev = 0;
      for (unsigned j = 0; j < result_j.size(); j++)
      {
        if (result_j.at(j) > 0)
        {
          std_dev += pow(result_j.at(j) - average, 2);
        }
      }
      std_dev = sqrt(std_dev / (n - 1));
      result_error.push_back(std_dev);
    }
    else
    {
      result_error.push_back(0);
      result.push_back(0);
    }
  }

  for (unsigned i = 0; i < result_error.size(); i++)
  {
    if (result_error.at(i) > 0)
      result.push_back(result_error.at(i));
    else
      result.push_back(0);
  }

  return result;

}

TH1F * CorrectHist( TH1F * h_reco, vector<Float_t> p_vec)
{
  const Int_t nbins = h_reco->GetNbinsX();

  TH1F *corrected = new TH1F("corrected", "corrected", 100,-1,1);
  corrected->Sumw2();
  for (int i = 1; i < nbins / 2 + 1; i++)
  {
    // float p = p_vec.at(i - 1);
    float p = 0.7;
    float q = 1 - p;
    float weight = (p * p + q * q) / (q * q * q * q - p * p * p * p);

    // calcualte average
    float av_i = 0;
    float av_41i = 0;
    float n = 0;
    for (int i1 = -1; i1 < 2; i1 += 1)
    {
      for (int i2 = -1; i2 < 2; i2 += 1)
      {
        float nm_reco_error = h_reco->GetBinContent(i) + i1 * h_reco->GetBinError(i);
        float np_reco_error = h_reco->GetBinContent(nbins + 1 - i) + i2 * h_reco->GetBinError(nbins + 1 - i);
        av_i += (np_reco_error * q * q - nm_reco_error * p * p) * weight;
        av_41i += -(np_reco_error * p * p - nm_reco_error * q * q) * weight;
        n++;
      }
    }
    av_i /= n;
    av_41i /= n;
    corrected->SetBinContent(i, av_i);
    corrected->SetBinContent(nbins + 1 - i, av_41i);

    // calculate error
    float error_i = 0;
    float error_41i = 0;
    n = 0;
    for (int i1 = -1; i1 < 2; i1 += 1)
    {
      for (int i2 = -1; i2 < 2; i2 += 1)
      {
        float nm_reco_error = h_reco->GetBinContent(i) + i1 * h_reco->GetBinError(i);
        float np_reco_error = h_reco->GetBinContent(nbins + 1 - i) + i2 * h_reco->GetBinError(nbins + 1 - i);
        error_i += pow((np_reco_error * q * q - nm_reco_error * p * p) * weight - av_i, 2);
        error_41i += pow(-(np_reco_error * p * p - nm_reco_error * q * q) * weight - av_41i, 2);
        n++;
      }
    }
    error_i = sqrt(error_i / (n - 1.));
    error_41i = sqrt(error_41i / (n - 1.));
    corrected->SetBinError(i, error_i);
    corrected->SetBinError(nbins + 1 - i, error_41i);
  }

  return corrected;

}

TH1F * Efficiency_Correction( TH1F * h, TString name, TFile * file )
{
  TH1F *h_gen_N_Pi_cos  = (TH1F*) file->Get("h_gen_N_Pi_cos");
  TH1F *h_reco_N_Pi_cos = (TH1F*) file->Get("h_reco_N_Pi_cos");
  TH1F *h_N_Pi_corr_cos = (TH1F*) file->Get("h_N_Pi_corr_cos");

  TH1F *h_stable_cos = (TH1F*) h_N_Pi_corr_cos->Clone();
  h_stable_cos->Divide(h_gen_N_Pi_cos);

  if( h->GetNbinsX() != h_stable_cos->GetNbinsX() ) throw std::logic_error("Error");

  Int_t nbins = h_stable_cos->GetNbinsX();
  TH1F *corrected = new TH1F(name.Data(), "corrected", 100,-1,1);
  corrected->Sumw2();
  for (int ibin = 1; ibin < nbins + 1; ibin++){

    Float_t binc_h   = h->GetBinContent(ibin);
    Float_t binc_eff = h_stable_cos->GetBinContent(ibin);

    corrected->SetBinContent(ibin,binc_h / binc_eff);

  }

  return corrected;

}

TH1F * Efficiency_Correction2( TH1F * h, TString name, TFile * file )
{
  TH1F *h_gen_N_Pi_cos  = (TH1F*) file->Get("h_gen_N_Pi_cos");
  TH1F *h_reco_N_Pi_cos = (TH1F*) file->Get("h_reco_N_Pi_cos");
  TH1F *h_N_Pi_corr_cos = (TH1F*) file->Get("h_N_Pi_corr_cos");

  TH1F *h_stable_cos = (TH1F*) h_N_Pi_corr_cos->Clone();
  TH1F *h_purity_cos = (TH1F*) h_N_Pi_corr_cos->Clone();
  h_stable_cos->Divide(h_gen_N_Pi_cos);
  h_purity_cos->Divide(h_reco_N_Pi_cos);

  if( h->GetNbinsX() != h_stable_cos->GetNbinsX() ) throw std::logic_error("Error");
  if( h->GetNbinsX() != h_purity_cos->GetNbinsX() ) throw std::logic_error("Error");

  TH1F *h_weight = (TH1F*) h_purity_cos->Clone();
  h_weight->Divide(h_stable_cos);

  Int_t nbins = h_stable_cos->GetNbinsX();
  TH1F *corrected = new TH1F(name.Data(), "corrected", 100,-1,1);
  corrected->Sumw2();
  for (int ibin = 1; ibin < nbins + 1; ibin++){

    Float_t binc_h   = h->GetBinContent(ibin);
    Float_t binc_eff = h_weight->GetBinContent(ibin);

    corrected->SetBinContent(ibin,binc_h * binc_eff);

  }

  return corrected;

}

void Func2Hist( TH1F * h, double * par )
{
  for ( int ibin=1; ibin < nbins_cos+1; ibin++ ){

    Float_t x = h->GetXaxis()->GetBinCenter(ibin);
    Float_t binc_h = par[0]*(1+x*x)+par[1]*x;
    h->SetBinContent(ibin,binc_h);

  }
}

void main_pq_BGFit( TFile *files[] )
{
  enum MixProcess {kUU,kDD,kUD};
  gStyle->SetOptStat(0);

  // gen uu/ss polar
  TH1F *h_gen_uu_qcos = (TH1F*) files[kUU]->Get("h_gen_q_qcos");
  TH1F *h_gen_dd_qcos = (TH1F*) files[kDD]->Get("h_gen_q_qcos");
  TH1F *h_gen_uu_qcos_scale = (TH1F*) h_gen_uu_qcos->Clone();
  TH1F *h_gen_dd_qcos_scale = (TH1F*) h_gen_dd_qcos->Clone();

  StyleHist(h_gen_dd_qcos,kBlack);
  h_gen_dd_qcos->SetFillStyle(0);
  h_gen_dd_qcos->SetLineStyle(2);

  // Normalize(h_gen_uu_qcos,1.0);
  // Normalize(h_gen_dd_qcos,1.0);

  // reco uu/ss polar
  TH1F *h_reco_uu_Pi_qcos = (TH1F*) files[kUU]->Get("h_reco_Pi_qcos");
  TH1F *h_reco_dd_Pi_qcos = (TH1F*) files[kDD]->Get("h_reco_Pi_qcos");

  Int_t n_uu_gen  = h_gen_uu_qcos_scale->GetEntries();
  Int_t n_dd_gen  = h_gen_dd_qcos_scale->GetEntries();
  Int_t n_uu_reco = h_reco_uu_Pi_qcos->GetEntries();
  Int_t n_dd_reco = h_reco_dd_Pi_qcos->GetEntries();

  Float_t eff_uu = (Float_t) n_uu_reco / (Float_t) n_uu_gen;
  Float_t eff_ss = (Float_t) n_dd_reco / (Float_t) n_dd_gen;

  cout << "uu = eff : reco : gen = " <<  eff_uu << " : " << n_uu_reco << " : " << n_uu_gen << "\n";
  cout << "ss = eff : reco : gen = " <<  eff_ss << " : " << n_dd_reco << " : " << n_dd_gen << "\n";

  h_gen_uu_qcos_scale->Scale(eff_uu);
  h_gen_dd_qcos_scale->Scale(eff_ss);

  TH1F *h_gen_ud_qcos = (TH1F*) h_gen_uu_qcos_scale->Clone();
  h_gen_ud_qcos->Add(h_gen_dd_qcos_scale);

  StyleHist(h_gen_ud_qcos,kGreen+1);
  h_gen_ud_qcos->SetFillStyle(0);
  h_gen_ud_qcos->SetLineStyle(2);

  StyleHist(h_gen_uu_qcos_scale,kOrange+7);
  h_gen_uu_qcos_scale->SetFillStyle(0);
  h_gen_uu_qcos_scale->SetLineStyle(2);
  // Normalize(h_gen_uu_qcos_scale,1.0);
  // h_gen_uu_qcos_scale->Scale(1.0 / (Float_t) (h_gen_uu_qcos_scale->Integral()) );

  StyleHist(h_gen_dd_qcos_scale,kBlack);
  h_gen_dd_qcos_scale->SetFillStyle(0);
  h_gen_dd_qcos_scale->SetLineStyle(2);
  // Normalize(h_gen_dd_qcos_scale,1.0);


  // reco us polar
  TH1F *h_reco_us_Pi_scos = (TH1F*) files[kUD]->Get("h_reco_Pi_scos");
  TH1F *h_reco_us_Pi_qcos = (TH1F*) files[kUD]->Get("h_reco_Pi_qcos");

  // efficiency correction
  TH1F *h_reco_us_Pi_scos_eff_corr = Efficiency_Correction2(h_reco_us_Pi_scos,"scos_corr",files[kUD]);
  TH1F *h_reco_us_Pi_qcos_eff_corr = Efficiency_Correction2(h_reco_us_Pi_qcos,"qcos_corr",files[kUD]);
  // TH1F *h_reco_us_Pi_scos_eff_corr = (TH1F*) h_reco_us_Pi_scos->Clone();
  // TH1F *h_reco_us_Pi_qcos_eff_corr = (TH1F*) h_reco_us_Pi_qcos->Clone();

  // used for pq correction
  TH1F *h_acc_PiPi_cos  = (TH1F*) files[kUD]->Get("pq/h_acc_PiPi_cos");
  TH1F *h_rej_PiPi_cos  = (TH1F*) files[kUD]->Get("pq/h_rej_PiPi_cos");

  TH1F *h_acc_PiPi_cos_eff_corr = Efficiency_Correction2(h_acc_PiPi_cos,"acc_corr",files[kUD]);
  TH1F *h_rej_PiPi_cos_eff_corr = Efficiency_Correction2(h_rej_PiPi_cos,"rej_corr",files[kUD]);
  // TH1F *h_acc_PiPi_cos_eff_corr = (TH1F*) h_acc_PiPi_cos->Clone();
  // TH1F *h_rej_PiPi_cos_eff_corr = (TH1F*) h_rej_PiPi_cos->Clone();

  StyleHist(h_reco_us_Pi_scos_eff_corr,kBlack);
  h_reco_us_Pi_scos_eff_corr->SetFillStyle(0);
  StyleHist(h_reco_us_Pi_qcos_eff_corr,kRed+2);
  StyleHist(h_acc_PiPi_cos_eff_corr,kRed+2);
  StyleHist(h_rej_PiPi_cos_eff_corr,kBlue+2);

  const Int_t nbins = h_reco_us_Pi_scos_eff_corr->GetNbinsX();

  TH1F *p_KK = new TH1F("p_KK", "p_KK", 50,0,1);
  p_KK->Sumw2();

  vector<Float_t> p_vec = GetP(h_acc_PiPi_cos_eff_corr, h_rej_PiPi_cos_eff_corr);

  for (unsigned i = 0; i < p_vec.size() / 2; i++)
  {
    p_KK->SetBinContent(nbins / 2 - i, p_vec.at(i));
    p_KK->SetBinError(nbins / 2 - i, p_vec.at(i + nbins / 2));
  }

  TH1F *h_reco_Pi_pq_cos = CorrectHist(h_reco_us_Pi_qcos_eff_corr, p_vec);
  StyleHist(h_reco_Pi_pq_cos,kBlue);

  // Float_t split_pt = 0.11;

  // GEN
  TF1 * f_gen_uu_front = new TF1("f_gen_uu_front","[0]*(1+x*x)+[1]*x",-0.8,-0.5);
  TF1 * f_gen_uu_full  = new TF1("f_gen_uu_full","[0]*(1+x*x)+[1]*x",-1.0,1.0);

  f_gen_uu_front->SetParNames("S","A");
  f_gen_uu_full->SetParNames("S","A");

  h_gen_uu_qcos_scale->Fit("f_gen_uu_front","MNRS");

  double gen_uu_par[2];

  f_gen_uu_front->GetParameters(gen_uu_par);
  f_gen_uu_full->SetParameters(gen_uu_par);


  TCanvas *c0 = new TCanvas("c0","c0",800,800);
  TPad *pad0 = new TPad("pad0", "pad0",0,0,1,1);
  StylePad(pad0,0,0.12,0,0.15);

  h_gen_uu_qcos_scale->Draw("h");
  f_gen_uu_full->Draw("same");


  // RECO
  TF1 * f_reco_uu_front = new TF1("f_reco_uu_front","[0]*(1+x*x)+[1]*x",-0.6,-0.4);
  TF1 * f_reco_uu_full  = new TF1("f_reco_uu_full","[0]*(1+x*x)+[1]*x",-1.0,1.0);

  f_reco_uu_front->SetParNames("S","A");
  f_reco_uu_full->SetParNames("S","A");

  f_reco_uu_front->SetParameters(gen_uu_par[0],gen_uu_par[1]);
  // f_reco_uu_front->SetParLimits(0,2E3,1E4);
  f_reco_uu_front->SetParLimits(1,-1E5,-1E4);

  h_reco_Pi_pq_cos->Fit("f_reco_uu_front","MNRS");

  double reco_uu_par[2];

  f_reco_uu_front->GetParameters(reco_uu_par);
  f_reco_uu_full->SetParameters(reco_uu_par);
  // f_reco_uu_full->SetParameters(1.03832E+04,-6.14863e+03);


  TCanvas *c1 = new TCanvas("c1","c1",800,800);
  TPad *pad1 = new TPad("pad1", "pad1",0,0,1,1);
  StylePad(pad1,0,0.12,0,0.15);

  h_reco_Pi_pq_cos->Draw("h");
  f_reco_uu_full->Draw("same");


  TCanvas *c2 = new TCanvas("c2","c2",800,800);
  TPad *pad2 = new TPad("pad2", "pad2",0,0,1,1);
  StylePad(pad2,0,0.12,0,0.15);

  h_reco_Pi_pq_cos->Draw("h");
  f_reco_uu_full->Draw("same");
  h_gen_uu_qcos_scale->Draw("hsame");
  f_gen_uu_full->Draw("same");


/*

  // Fitting 1st Round
  // cos < -0.4
  // Float_t split_pt = -0.19;
  Float_t split_pt = 0.11;

  TF1 * f_dd_front = new TF1("f_dd_front","[0]*(1+x*x)+[1]*x",split_pt,0.7);
  TF1 * f_dd_full  = new TF1("f_dd_full","[0]*(1+x*x)+[1]*x",-1.0,1.0);

  f_dd_front->SetParNames("S","A");
  f_dd_full->SetParNames("S","A");

  h_reco_Pi_pq_cos->Fit("f_dd_front","MNRS");

  double ss_par[2];

  f_dd_front->GetParameters(ss_par);
  f_dd_full->SetParameters(ss_par);

  // function to histogram
  TH1F * h_f_dd_full = new TH1F("h_f_dd_full", "h_f_dd_full", 100,-1,1);
  Func2Hist(h_f_dd_full,ss_par);

  TH1F* h_reco_Pi_pq_cos_subtracted_back = (TH1F*) h_reco_Pi_pq_cos->Clone();
  for ( int ibin=1; ibin < nbins_cos+1; ibin++ ){

    Float_t x = h_reco_Pi_pq_cos_subtracted_back->GetXaxis()->GetBinCenter(ibin);
    // if( split_pt < x ) {
    //   h_reco_Pi_pq_cos_remain_back->SetBinContent(ibin,0);
    //   continue;
    // }
    Float_t bin_content_main  = h_reco_Pi_pq_cos_subtracted_back->GetBinContent(ibin);
    Float_t bin_content_sfunc = h_f_dd_full->GetBinContent(ibin);
    Float_t bin_content_subtracted = bin_content_main - bin_content_sfunc;

    h_reco_Pi_pq_cos_subtracted_back->SetBinContent(ibin,bin_content_subtracted);

  }

  TH1F* h_reco_Pi_pq_cos_remain_back = (TH1F*) h_reco_Pi_pq_cos->Clone();
  // subtracted cos < -0.4 region
  for ( int ibin=1; ibin < nbins_cos+1; ibin++ ){

    Float_t x = h_reco_Pi_pq_cos_remain_back->GetXaxis()->GetBinCenter(ibin);
    if( split_pt < x ) continue;
    Float_t bin_content_main  = h_reco_Pi_pq_cos_remain_back->GetBinContent(ibin);
    Float_t bin_content_sfunc = h_reco_Pi_pq_cos_subtracted_back->GetBinContent(ibin);
    Float_t bin_content_subtracted = bin_content_main - bin_content_sfunc;

    h_reco_Pi_pq_cos_remain_back->SetBinContent(ibin,bin_content_subtracted);

  }

  TCanvas *c1 = new TCanvas("c1","c1",800,800);
  TPad *pad1 = new TPad("pad1", "pad1",0,0,1,1);
  StylePad(pad1,0,0.12,0,0.15);

  TH1F * h_gen_uu_qcos_sample = (TH1F*) h_gen_uu_qcos_scale->Clone();
  h_gen_uu_qcos_sample->Scale( h_reco_Pi_pq_cos_subtracted_back->Integral(11,30) / h_gen_uu_qcos_sample->Integral(11,30) );

  h_reco_Pi_pq_cos_subtracted_back->GetXaxis()->SetRangeUser(-1,0);
  h_reco_Pi_pq_cos_subtracted_back->GetYaxis()->SetRangeUser(0,13E3);
  h_reco_Pi_pq_cos_subtracted_back->SetTitle(";cos#theta;Entries");
  h_reco_Pi_pq_cos_subtracted_back->Draw("h");
  h_gen_uu_qcos_sample->Draw("hsame");

  // -0.4 < cos
  TF1 * f_uu_back = new TF1("f_uu_back","[0]*(1+x*x)+[1]*x",-0.75,split_pt);
  TF1 * f_uu_full = new TF1("f_uu_full","[0]*(1+x*x)+[1]*x",-1.0,1.0);

  f_uu_back->SetParNames("S","A");

  h_reco_Pi_pq_cos_subtracted_back->Fit("f_uu_back","MNR");

  double uu_par[2];
  f_uu_back->GetParameters(uu_par);
  f_uu_full->SetParameters(uu_par);

  f_uu_full->Draw("same");


  TH1F * h_f_uu_full = new TH1F("h_f_uu_full", "h_f_uu_full", 100,-1,1);
  Func2Hist(h_f_uu_full,uu_par);


  TH1F* h_reco_Pi_pq_cos_remain_front = (TH1F*) h_reco_Pi_pq_cos_remain_back->Clone();
  for ( int ibin=1; ibin < nbins_cos+1; ibin++ ){

    Float_t x = h_reco_Pi_pq_cos_remain_front->GetXaxis()->GetBinCenter(ibin);
    if( x < split_pt ) continue;
    Float_t bin_content_main  = h_reco_Pi_pq_cos_remain_front->GetBinContent(ibin);
    Float_t bin_content_sfunc = h_f_uu_full->GetBinContent(ibin);
    Float_t bin_content_subtracted = bin_content_main - bin_content_sfunc;

    h_reco_Pi_pq_cos_remain_front->SetBinContent(ibin,bin_content_subtracted);

  }

  // TH1F* h_reco_Pi_pq_cos_remain_front = (TH1F*) h_reco_Pi_pq_cos_remain_back->Clone();
  // h_reco_Pi_pq_cos_remain_front->Add(h_reco_Pi_pq_cos_diff_front,-1);      // subtracted -0.4 < cos region
  StyleHist(h_reco_Pi_pq_cos_remain_front,kViolet);



  // Normalize(h_reco_Pi_pq_cos_remain_front,1.0);

  // Int_t scale_sum = h_gen_uu_qcos_scale->GetEntries();
  // h_gen_uu_qcos_scale->Scale( 1.8 / (Float_t) scale_sum );

  double intgen  = h_gen_ud_qcos->Integral(70,80);
  double intreco = h_reco_Pi_pq_cos->Integral(70,80);
  // h_gen_ud_qcos->Scale(intreco/intgen);

  double intgen2  = h_gen_dd_qcos_scale->Integral(90,95);
  double intreco2 = h_gen_ud_qcos->Integral(90,95);
  // h_gen_dd_qcos_scale->Scale(intreco2/intgen2);

  double intgen3  = h_gen_uu_qcos_scale->Integral(5,10);
  double intreco3 = h_gen_ud_qcos->Integral(5,10);
  // h_gen_uu_qcos_scale->Scale(intreco3/intgen3);


  TCanvas *c0 = new TCanvas("c0","c0",800,800);
  TPad *pad0 = new TPad("pad0", "pad0",0,0,1,1);
  StylePad(pad0,0,0.12,0,0.15);

  h_reco_Pi_pq_cos->GetYaxis()->SetRangeUser(0,40E3);
  h_reco_Pi_pq_cos->SetTitle(";K^{+}K^{-} cos#theta;Entries");
  h_reco_Pi_pq_cos->Draw("h");
  h_reco_us_Pi_qcos_eff_corr->Draw("hsame");
  h_reco_us_Pi_scos_eff_corr->Draw("hsame");

  h_reco_Pi_pq_cos_remain_front->Draw("hsame");
  h_gen_ud_qcos->Draw("hsame");
  h_gen_dd_qcos_scale->Draw("hsame");
  h_gen_uu_qcos_scale->Draw("hsame");

  f_dd_full->Draw("same");
  // f_uu_full->Draw("same");
  

  TLegend *leg = new TLegend(0.2,0.70,0.7,0.85);
  leg->SetLineColor(0);
  leg->AddEntry(h_gen_ud_qcos,"Gen #bar{u}/d-quark angle","l");
  leg->AddEntry(h_gen_uu_qcos_scale,"Gen #bar{u}-quark angle","l");
  leg->AddEntry(h_gen_dd_qcos_scale,"Gen d-quark angle","l");
  leg->AddEntry(h_reco_us_Pi_scos_eff_corr,"Reco #pi^{+}#pi^{-} matched with #bar{u}/d-quark angle","l");
  leg->AddEntry(h_reco_us_Pi_qcos_eff_corr,"Reco #pi^{+}#pi^{-}","l");
  leg->AddEntry(h_reco_Pi_pq_cos,"Reco #pi^{+}#pi^{-} (pq correction)","l");
  // leg->AddEntry(h_reco_Pi_pq_cos_remain_front,"Reco K^{+}K^{-} (pq + FB-Fitting correction @ cos#theta = -0.4)","l");
  leg->Draw();

  */


}

void pq_method_PiBGFit_ud()
{
  TGaxis::SetMaxDigits(3);

  try
  {
    TString process[3] = {"uu","dd","ud"};
    TFile* files[3];
    for ( int i=0; i<3; i++ ){
      files[i] = new TFile(TString::Format("../rootfiles/merged/rv02-02.sv02-02.mILD_l5_o1_v02.E250-SetA.I500010.P2f_z_h.eL.pR.%s.KPiLPFO.distPi0.PFOp15.LPFOp15_pNaN.tpc0.hists.all.root",process[i].Data()),"READ");
      if ( !files[i]->IsOpen() ) throw 0;
    }

    main_pq_BGFit( files );
  }
  catch ( int error_code ) {
    switch ( error_code ){
      default:
        cerr << "<< Error >>" << endl;
        break;
    }
  }

}